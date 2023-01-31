#include <search/Search/MCTS.hpp>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <effolkronium/random.hpp>
#include <limits>
#include <random>

#include <iostream>

#include <search/NN/NNManager.hpp>
#include <search/Utils/Utils.hpp>

namespace search::Search
{
float SimulationResult::Convert(fights::Player pla) const
{
    if (player == pla)
        return value;
    return -value;
}

SimulationResult SimulationResult::FromValueNet(float val, fights::Player pla)
{
    SimulationResult res;
    res.value = val;
    res.player = pla;
    return res;
}

SimulationResult SimulationResult::FromGameResult(fights::Player winner)
{
    SimulationResult res;
    res.value = 1;
    res.player = winner;
    return res;
}

MCTS::MCTS(Config config) : config_(config), controller_(config)
{
    controller_.Pause();

    workers_.resize(config_.search.NumWorkers);
    for (int rank = 0; rank < config_.search.NumWorkers; ++rank)
    {
        workers_[rank] = std::thread(&MCTS::workerThread, this);
    }
    controller_.WaitAllPaused();

    deleteWorker_ = std::thread(&MCTS::deleteThread, this);

    updateRoot(nullptr);
}

MCTS::~MCTS() noexcept
{
    controller_.Shutdown();
    for (auto& worker : workers_)
        if (worker.joinable())
            worker.join();

    enqDeleteNode(root_);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    {
        std::scoped_lock lock(deleteMutex_);
        runningDeleteWorker_ = false;
        cv_.notify_all();
    }
    if (deleteWorker_.joinable())
        deleteWorker_.join();
}

void MCTS::DoSearchWithMaxSimulation()
{
    controller_.WaitAllPaused();
    initSearch();

    while (numSimulations_.load() < config_.search.MaxSimulation)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    controller_.Pause();
}

void MCTS::Play(Game::Action action)
{
    controller_.Pause();
    controller_.WaitAllPaused();

    mainEnv_.Play(*action.action);

    MCTSNode* newRoot = nullptr;
    root_->ForEachChild([&newRoot, action](MCTSNode* child) {
        if (child->action.id == action.id)
        {
            newRoot = child;
        }
    });

    updateRoot(newRoot);
}

Game::Action MCTS::GetBestAction() const
{
    std::uint64_t bestVisits = 0;
    MCTSNode* bestNode = nullptr;

    root_->ForEachChild([&bestVisits, &bestNode](MCTSNode* child) {
        const std::uint64_t visits = child->visits;

        if (visits >= bestVisits)
        {
            bestVisits = visits;
            bestNode = child;
        }
    });

    assert(bestNode != nullptr);
    return bestNode->action;
}

const MCTSNode* MCTS::GetRoot() const
{
    return root_;
}

void MCTS::workerThread()
{
    while (true)
    {
        if (controller_.GetState() == MCTSState::IDLE)
        {
            controller_.AckPause();
            controller_.WaitResume();
            controller_.AckResume();
        }

        if (controller_.GetState() == MCTSState::SHUTDOWN)
        {
            break;
        }

        // search routine
        Game::Environment env(mainEnv_);
        simulation(env, root_);
    }
}

void MCTS::deleteThread()
{
    while (true)
    {
        std::unique_lock lock(deleteMutex_);

        cv_.wait(lock, [&] {
            return !runningDeleteWorker_ || !deleteQueue_.empty();
        });
        if (!runningDeleteWorker_ && deleteQueue_.empty())
        {
            break;
        }

        MCTSNode* nodeToDelete = deleteQueue_.front();
        deleteQueue_.pop_front();

        deleteNode(nodeToDelete);
        delete nodeToDelete;
    }
}

void MCTS::initSearch()
{
    numSimulations_ = 0;
    initRoot();

    controller_.Resume();
    controller_.WaitAllResumed();
}

void MCTS::updateRoot(MCTSNode* newNode)
{
    MCTSNode* node = new MCTSNode;
    if (newNode == nullptr)
    {
        node = new MCTSNode;
        node->player = mainEnv_.GetOpponentPlayer();
    }
    else
    {
        node->state = newNode->state.load();
        node->action = newNode->action;
        node->player = newNode->player;
        node->numChildren = newNode->numChildren;
        node->policy = newNode->policy;
        node->visits = newNode->visits.load();
        node->values = newNode->values.load();
        node->virtualLoss = newNode->virtualLoss.load();
        node->parentNode = newNode->parentNode;
        node->mostLeftChildNode = newNode->mostLeftChildNode;
        node->rightSiblingNode = newNode->rightSiblingNode;

        node->ForEachChild(
            [node](MCTSNode* child) { child->parentNode = node; });

        node->parentNode = nullptr;

        newNode->state = ExpandState::UNEXPANDED;
        newNode->mostLeftChildNode = nullptr;
        newNode->numChildren = 0;
    }

    if (root_ != nullptr)
    {
        enqDeleteNode(root_);
    }

    root_ = node;
}

void MCTS::initRoot()
{
    if (root_->state == ExpandState::UNEXPANDED)
    {
        NN::NNOutput nnEval = NN::NNManager::GetInstance().Predict(mainEnv_);

        root_->Expand(config_, mainEnv_, nnEval);
    }

    if (config_.search.EnableDirichletNoise)
    {
        using Random = effolkronium::random_static;

        std::gamma_distribution<float> dist(config_.search.DirichletNoiseAlpha);
        NN::PolicyVal noise;

        for (auto& v : noise)
        {
            v = Random::get(dist);
        }

        const float noiseSum =
            std::accumulate(begin(noise), begin(noise) + root_->numChildren,
                            std::numeric_limits<float>::epsilon());

        float policySum = std::numeric_limits<float>::epsilon();
        std::uint64_t idx = 0;
        root_->ForEachChild([&noise, noiseSum, &policySum, &idx,
                             this](MCTSNode* child) {
            child->policy = (config_.search.DirichletNoiseEps) * child->policy +
                            (1.f - config_.search.DirichletNoiseEps) *
                                noise[idx] / noiseSum;

            policySum += child->policy;
        });

        root_->ForEachChild(
            [policySum](MCTSNode* child) { child->policy /= policySum; });
    }
}

void MCTS::enqDeleteNode(MCTSNode* node)
{
    std::scoped_lock lock(deleteMutex_);
    deleteQueue_.emplace_back(node);
}

void MCTS::deleteNode(MCTSNode* node)
{
    node->ForEachChild([this](MCTSNode* child) { deleteNode(child); });

    MCTSNode* tempNowNode = node->mostLeftChildNode;
    MCTSNode* nodeToDelete = nullptr;
    while (tempNowNode != nullptr)
    {
        nodeToDelete = tempNowNode;
        tempNowNode = tempNowNode->rightSiblingNode;

        delete nodeToDelete;
    }

    node->mostLeftChildNode = nullptr;
}

void MCTS::simulation(Game::Environment& env, MCTSNode* node)
{
    while (node->state == ExpandState::EXPANDED)
    {
        MCTSNode* tmpNode = node;
        tmpNode = node->Select(config_);

        if (tmpNode == nullptr)
        {
            std::cout << env.ToString() << std::endl;

            for (auto act : env.GetHistory())
            {
                std::cout << act->ToString() << " ";
            }
            std::cout << std::endl;
            std::cout << "AVAILS: " << env.GetValidActions().size() << " "
                      << static_cast<int>(node->state.load()) << " "
                      << node->numChildren << " " << node->mostLeftChildNode
                      << std::endl;

            node = nullptr;
        }
        else
        {
            node = tmpNode;
        }

        env.Play(*node->action.action, node->player);
        Utils::AtomicAdd(node->virtualLoss, config_.search.VirtualLoss);
    }

    if (node->state == ExpandState::EXPANDING)
        return;

    const auto winner = env.GetWinner();

    SimulationResult result;
    if (env.IsEnd())
    {
        result = SimulationResult::FromGameResult(winner);
    }
    else
    {
        NN::NNOutput nnEval = NN::NNManager::GetInstance().Predict(env);
        node->Expand(config_, env, nnEval);

        result = SimulationResult::FromValueNet(nnEval.value,
                                                env.GetCurrentPlayer());
    }

    while (node != nullptr)
    {
        Utils::AtomicAdd(node->values, result.Convert(node->player));
        ++node->visits;

        if (node != root_)
        {
            Utils::AtomicAdd(node->virtualLoss, -config_.search.VirtualLoss);
        }

        node = node->parentNode;
    }

    ++numSimulations_;
}
}  // namespace search::Search
