#pragma once

#include <condition_variable>
#include <deque>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <search/Config.hpp>
#include <search/Game/Environment.hpp>
#include <search/NN/NNBase.hpp>
#include <search/Utils/WaitGroup.hpp>

namespace search::NN
{
struct Task final
{
    Game::Environment env;
    std::promise<NNOutput> eval;
};

class NNManager final
{
 public:
    static NNManager& GetInstance();

 public:
    NNManager(Config config);
    ~NNManager() noexcept;

    NNOutput Predict(const Game::Environment& env);

 private:
    void workerThread(int rank);

 private:
    inline static NNManager* instance_{ nullptr };

    bool engineChanged_{ false };
    bool running_{ false };

    Config config_;
    std::vector<std::thread> workers_;

    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<Task> taskQueue_;

    Utils::WaitGroup wg_;
};
}  // namespace search::NN
