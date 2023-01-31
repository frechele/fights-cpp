#include <search/Search/MCTSController.hpp>

namespace search::Search
{
MCTSController::MCTSController(const Config& config)
    : numWorkers_(config.search.NumWorkers)
{
}

void MCTSController::Resume()
{
    std::scoped_lock lock(mutex_);
    if (state_ == MCTSState::SEARCHING)
    {
        return;
    }

    for (int i = 0; i < numWorkers_; ++i)
    {
        resumeWg_.Add();
    }

    state_ = MCTSState::SEARCHING;
    cv_.notify_all();
}

void MCTSController::Pause()
{
    std::scoped_lock lock(mutex_);
    if (state_ == MCTSState::IDLE)
    {
        return;
    }

    for (int i = 0; i < numWorkers_; ++i)
    {
        pauseWg_.Add();
    }

    state_ = MCTSState::IDLE;
}

void MCTSController::Shutdown()
{
    std::scoped_lock lock(mutex_);

    state_ = MCTSState::SHUTDOWN;
    cv_.notify_all();
}

void MCTSController::WaitResume()
{
    std::unique_lock lock(mutex_);

    cv_.wait(lock, [this] { return state_ != MCTSState::IDLE; });
}

void MCTSController::WaitAllResumed()
{
    resumeWg_.Wait();
}

void MCTSController::WaitAllPaused()
{
    pauseWg_.Wait();
}

void MCTSController::AckResume()
{
    resumeWg_.Done();
}

void MCTSController::AckPause()
{
    pauseWg_.Done();
}

MCTSState MCTSController::GetState() const
{
    std::scoped_lock lock(mutex_);
    return state_;
}
}  // namespace search::Search
