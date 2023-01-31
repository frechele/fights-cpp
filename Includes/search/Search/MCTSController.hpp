#pragma once

#include <condition_variable>
#include <mutex>

#include <search/Config.hpp>
#include <search/Utils/WaitGroup.hpp>

namespace search::Search
{
enum class MCTSState
{
    IDLE,
    SEARCHING,
    SHUTDOWN
};

class MCTSController final
{
 public:
    MCTSController(const Config& config);

    void Resume();
    void Pause();
    void Shutdown();

    void WaitResume();

    void WaitAllResumed();
    void WaitAllPaused();

    void AckResume();
    void AckPause();

    MCTSState GetState() const;

 private:
    std::condition_variable cv_;
    mutable std::mutex mutex_;
    MCTSState state_{ MCTSState::IDLE };

    int numWorkers_;
    Utils::WaitGroup resumeWg_, pauseWg_;
};
};  // namespace search::Search
