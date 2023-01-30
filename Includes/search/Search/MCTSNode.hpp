#pragma once

#include <atomic>
#include <cstdint>

#include <search/Config.hpp>
#include <search/Game/Environment.hpp>
#include <search/NN/NNBase.hpp>

namespace search::Search
{
enum class ExpandState
{
    UNEXPANDED,
    EXPANDING,
    EXPANDED
};

struct MCTSNode final
{
    MCTSNode() = default;
    MCTSNode(MCTSNode&& other);

    std::atomic<ExpandState> state{ ExpandState::UNEXPANDED };

    Game::Action action;
    fights::Player player;

    int numChildren{ 0 };
    float policy{ 0 };
    std::atomic<std::uint64_t> visits{ 0 };
    std::atomic<float> values{ 0 };
    std::atomic<float> virtualLoss{ 0 };

    MCTSNode* parentNode{ nullptr };
    MCTSNode* mostLeftChildNode{ nullptr };
    MCTSNode* rightSiblingNode{ nullptr };

    MCTSNode* Select(const Config& config) const;
    void Expand(const Config& config, const Game::Environment& env,
                const NN::NNOutput& nnOutput);
};
}  // namespace search::Search
