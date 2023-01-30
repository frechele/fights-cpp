#include <search/NN/NNManager.hpp>

#include <algorithm>
#include <cassert>
#include <memory>

#include <search/NN/FakeNet.hpp>

namespace search::NN
{
NNManager& NNManager::GetInstance()
{
    return *instance_;
}

NNManager::NNManager(Config config) : config_(std::move(config))
{
    assert(instance_ == nullptr);
    instance_ = this;

    running_ = true;
    workers_.resize(config_.nn.NumWorkers);
    for (int rank = 0; rank < config_.nn.NumWorkers; ++rank)
    {
        wg_.Add();
        workers_[rank] = std::thread(&NNManager::workerThread, this, rank);
    }
}

NNManager::~NNManager() noexcept
{
    running_ = false;

    for (auto& worker : workers_)
        if (worker.joinable())
            worker.join();
}

NNOutput NNManager::Predict(const Game::Environment& env)
{
    Task task;
    task.state = EnvToState(env);
    auto outputFuture = task.eval.get_future();

    {
        std::scoped_lock lock(mutex_);

        taskQueue_.emplace_back(std::move(task));
    }

    return outputFuture.get();
}

void NNManager::workerThread(int rank)
{
    std::unique_ptr<NNBase> network;

    if (config_.nn.EngineType == "fake")
    {
        network = std::make_unique<FakeNet>();
    }

    network->Initialize(rank, config_.nn.EngineFile);
    wg_.Done();

    while (true)
    {
        int batchSize;
        std::vector<Tensor> states;
        std::vector<std::promise<NNOutput>> results;
        {
            std::scoped_lock lock(mutex_);

            if (!running_ && taskQueue_.empty())
            {
                break;
            }

            if (taskQueue_.empty())
            {
                continue;
            }

            batchSize =
                std::min<int>(config_.nn.MaxBatch, taskQueue_.size());

            for (int batchID = 0; batchID < batchSize; ++batchID)
            {
                Task task = std::move(taskQueue_.front());
                taskQueue_.pop_front();

                states.emplace_back(std::move(task.state));
                results.emplace_back(std::move(task.eval));
            }
        }

        auto outputs = network->Predict(states);

        for (int batchID = 0; batchID < batchSize; ++batchID)
        {
            results[batchID].set_value(std::move(outputs[batchID]));
        }
    }

    network->Shutdown();
}
}  // namespace search::NN
