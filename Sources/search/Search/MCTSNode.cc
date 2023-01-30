#include <search/Search/MCTSNode.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

namespace search::Search
{
MCTSNode::MCTSNode(MCTSNode&& other)
    : state(other.state.load()),
      action(other.action),
      player(other.player),
      numChildren(other.numChildren),
      policy(other.policy),
      visits(other.visits.load()),
      values(other.values.load()),
      virtualLoss(other.virtualLoss.load()),
      parentNode(other.parentNode),
      mostLeftChildNode(other.mostLeftChildNode),
      rightSiblingNode(other.rightSiblingNode)
{
    other.state = ExpandState::UNEXPANDED;
    other.mostLeftChildNode = nullptr;
    other.numChildren = 0;
}

MCTSNode* MCTSNode::Select(const Config& config) const
{
    while (state.load() == ExpandState::EXPANDING)
        ;

    assert(mostLeftChildNode != nullptr);

    float totalParentVisits = 0;
    for (MCTSNode* tempNowNode = mostLeftChildNode; tempNowNode != nullptr;
         tempNowNode = tempNowNode->rightSiblingNode)
    {
        totalParentVisits += tempNowNode->visits;
    }

    float maxValue = -std::numeric_limits<float>::max();
    MCTSNode* bestNode = nullptr;

    ForEachChild([&maxValue, &bestNode, &config, totalParentVisits,
                  this](MCTSNode* child) {
        float Q;
        // calcuate Q-value
        {
            const float v = child->visits;
            const float w = child->values;
            const float vl = child->virtualLoss;

            Q = (w - vl - 1.f) / (v + vl + 1.f);
        }

        float u;
        // calculate u-value
        {
            const float p = child->policy;
            const float v = child->visits;

            u = config.search.cPUCT * p * std::sqrt(totalParentVisits) /
                (1.f + v);
        }

        const float value = Q + u;

        if (maxValue < value)
        {
            maxValue = value;
            bestNode = child;
        }
    });

    assert(bestNode != nullptr);
    return bestNode;
}

void MCTSNode::Expand(const Config& config, const Game::Environment& env,
                      const NN::NNOutput& nnOutput)
{
    {
        ExpandState expected = ExpandState::UNEXPANDED;
        if (!this->state.compare_exchange_weak(expected,
                                               ExpandState::EXPANDING))
            return;
    }

    const fights::Player player = env.GetCurrentPlayer();

    // filtering only valid actions
    auto actionList = env.GetValidActions();

    float probSum = std::numeric_limits<float>::epsilon();
    for (const auto action : actionList)
    {
        probSum += nnOutput.policy[action.id];
    }

    std::stable_sort(begin(actionList), end(actionList),
                     [&nnOutput](Game::Action a, Game::Action b) {
                         return nnOutput.policy[a.id] > nnOutput.policy[b.id];
                     });

    MCTSNode* nowNode = nullptr;
    for (const auto action : actionList)
    {
        MCTSNode* node = new MCTSNode;
        node->player = player;
        node->action = action;
        node->policy = nnOutput.policy[action.id] / probSum;

        if (nowNode == nullptr)
            mostLeftChildNode = node;
        else
            nowNode->rightSiblingNode = node;

        ++numChildren;
        node->parentNode = this;
        nowNode = node;

        if (numChildren == config.search.MaxAction)
            break;
    }

    state = ExpandState::EXPANDED;
}
}  // namespace search::Search
