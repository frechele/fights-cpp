#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

#include <search/Config.hpp>
#include <search/Game/Environment.hpp>
#include <search/Search/MCTSNode.hpp>
#include <search/Utils/WaitGroup.hpp>

namespace search::Search
{
enum class MCTSStatus
{
    IDLE,
    SEARCHING,
    SHUTDOWN
};

struct SimulationResult final
{
    float value{ 0 };
    fights::Player player{ fights::Player::NONE };

    float Convert(fights::Player pla) const;

    static SimulationResult FromValueNet(float val, fights::Player pla);
    static SimulationResult FromGameResult(fights::Player winner);
};

class MCTS final
{
 public:
    MCTS(Config config);
    ~MCTS() noexcept;

    void DoSearchWithMaxSimulation();

    void Play(Game::Action action);

    Game::Action GetBestAction() const;
    const MCTSNode* GetRoot() const;

 private:
    void workerThread();
    void deleteThread();

    void startSearch();
    void stopSearch();
    void waitAllSearchStopped();

    void updateRoot(MCTSNode* newNode);
    void initRoot();

    void enqDeleteNode(MCTSNode* node);
    void deleteNode(MCTSNode* node);

    void simulation(Game::Environment& env, MCTSNode* node);

 private:
    Config config_;

    Game::Environment mainEnv_;
    MCTSNode* root_{ nullptr };

    std::vector<std::thread> workers_;

    std::condition_variable cv_;
    std::mutex mutex_;
    MCTSStatus status_{ MCTSStatus::IDLE };
    Utils::WaitGroup wg_;

    std::atomic<std::uint64_t> numSimulations_{ 0 };

    std::deque<MCTSNode*> deleteQueue_;
    std::mutex deleteMutex_;
    std::thread deleteWorker_;
    bool runningDeleteWorker_{ true };
};
}  // namespace search::Search
