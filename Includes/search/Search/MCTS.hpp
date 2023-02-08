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
#include <search/Search/MCTSController.hpp>
#include <search/Search/MCTSNode.hpp>
#include <search/Utils/WaitGroup.hpp>

namespace search::Search
{
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

    const Config& GetConfig() const;
    Config& GetConfig();

    void DumpStats() const;

 private:
    void workerThread();
    void deleteThread();

    void initSearch();

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

    MCTSController controller_;

    std::atomic<std::uint64_t> numSimulations_{ 0 };

    std::atomic<MCTSNode*> deleteNodeHead_{ nullptr };
    std::thread deleteWorker_;
    bool runningDeleteWorker_{ true };
};
}  // namespace search::Search
