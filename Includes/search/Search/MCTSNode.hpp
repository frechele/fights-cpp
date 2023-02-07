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
    std::atomic<ExpandState> state{ ExpandState::UNEXPANDED };

    Game::Action action;
    fights::Player player;
    std::uint64_t hash{ 0 };

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

    template <typename Func>
    void ForEachChild(Func&& f);

    template <typename Func>
    void ForEachChild(Func&& f) const;
};

template <typename Func>
void MCTSNode::ForEachChild(Func&& f)
{
    for (MCTSNode* tempNowNode = mostLeftChildNode; tempNowNode != nullptr;
         tempNowNode = tempNowNode->rightSiblingNode)
        f(tempNowNode);
}

template <typename Func>
void MCTSNode::ForEachChild(Func&& f) const
{
    for (MCTSNode* tempNowNode = mostLeftChildNode;
         tempNowNode != nullptr; tempNowNode = tempNowNode->rightSiblingNode)
        f(tempNowNode);
}
}  // namespace search::Search
