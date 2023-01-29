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
    {
        std::scoped_lock lock(mutex_);
        running_ = false;
        cv_.notify_all();
    }

    for (auto& worker : workers_)
        if (worker.joinable())
            worker.join();
}

NNOutput NNManager::Predict(const Game::Environment& env)
{
    std::future<NNOutput> outputFuture;

    {
        std::scoped_lock lock(mutex_);

        Task task;
        task.env = env;

        outputFuture = task.eval.get_future();
        taskQueue_.emplace_back(std::move(task));
        cv_.notify_one();
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
        std::unique_lock lock(mutex_);

        cv_.wait(lock, [&] { return !running_ || !taskQueue_.empty(); });
        if (!running_ && taskQueue_.empty())
        {
            break;
        }

        const int batchSize =
            std::min<int>(config_.nn.MaxBatch, taskQueue_.size());

        std::vector<Task> buffer(batchSize);
        for (int batchID = 0; batchID < batchSize; ++batchID)
        {
            buffer[batchID] = std::move(taskQueue_.front());
            taskQueue_.pop_front();
        }

        lock.unlock();

        std::vector<Game::Environment> states(batchSize);
        for (int batchID = 0; batchID < batchSize; ++batchID)
        {
            states[batchID] = std::move(buffer[batchID].env);
        }

        auto outputs = network->Predict(states);

        for (int batchID = 0; batchID < batchSize; ++batchID)
        {
            buffer[batchID].eval.set_value(std::move(outputs[batchID]));
        }
    }

    network->Shutdown();
}
}  // namespace search::NN
