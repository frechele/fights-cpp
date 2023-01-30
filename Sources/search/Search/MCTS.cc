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

MCTS::MCTS(Config config)
    : config_(config)
{
    workers_.resize(config_.search.NumWorkers);
    for (int rank = 0; rank < config_.search.NumWorkers; ++rank)
    {
        wg_.Add();
        workers_[rank] = std::thread(&MCTS::workerThread, this);
    }
    waitAllSearchStopped();

    deleteWorker_ = std::thread(&MCTS::deleteThread, this);

    updateRoot(nullptr);
}

MCTS::~MCTS() noexcept
{
    {
        std::scoped_lock lock(mutex_);
        status_ = MCTSStatus::SHUTDOWN;
    }
    cv_.notify_all();

    for (auto& worker : workers_)
        if (worker.joinable())
            worker.join();

    enqDeleteNode(root_);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    runningDeleteWorker_ = false;
    if (deleteWorker_.joinable())
        deleteWorker_.join();
}

void MCTS::DoSearchWithMaxSimulation()
{
    // waitAllSearchStopped();
    // startSearch();

    initRoot();
    numSimulations_ = 0;

    while (numSimulations_.load() < config_.search.MaxSimulation)
    {
        Game::Environment env(mainEnv_);
        simulation(env, root_);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // stopSearch();

    std::cout << "DONE SIMULATION " << numSimulations_.load() << std::endl;
}

void MCTS::Play(Game::Action action)
{
    stopSearch();

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
        {
            std::unique_lock lock(mutex_);
            if (status_ != MCTSStatus::SEARCHING)
                wg_.Done();
            cv_.wait(lock, [&] { return status_ != MCTSStatus::IDLE; });
        }

        if (status_ == MCTSStatus::SHUTDOWN)
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

void MCTS::startSearch()
{
    waitAllSearchStopped();

    if (status_ != MCTSStatus::IDLE)
        return;

    initRoot();

    {
        std::scoped_lock lock(mutex_);

        numSimulations_ = 0;

        status_ = MCTSStatus::SEARCHING;
    }
    cv_.notify_all();
}

void MCTS::stopSearch()
{
    for (int i = 0; i < config_.search.NumWorkers; ++i)
        wg_.Add();

    status_ = MCTSStatus::IDLE;
}

void MCTS::waitAllSearchStopped()
{
    assert(status_ == MCTSStatus::IDLE);

    wg_.Done();
}

void MCTS::updateRoot(MCTSNode* newNode)
{
    MCTSNode* node;
    if (newNode == nullptr)
    {
        node = new MCTSNode;
        node->player = mainEnv_.GetOpponentPlayer();
    }
    else
    {
        node = new MCTSNode(std::move(*newNode));

        node->ForEachChild(
            [node](MCTSNode* child) { child->parentNode = node; });

        node->parentNode = nullptr;
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
    /*
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
    */
}

void MCTS::simulation(Game::Environment& env, MCTSNode* node)
{
    /*
    while (node->state == ExpandState::EXPANDED)
    {
        node = node->Select(config_);

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
    */

    ++numSimulations_;
}
}  // namespace search::Search
