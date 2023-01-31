#pragma once

#include <vector>

#include <fights/Action.hpp>
#include <fights/Game.hpp>

#include <search/Game/ActionSpace.hpp>

namespace search::Game
{
class Environment : public fights::Game
{
 public:
    static constexpr int ACTION_SPACE_SIZE = 172;

 public:
    const std::vector<Action>& GetActionSpace() const;
    std::vector<Action> GetValidActions() const;

 private:
    inline static const ActionSpace actionSpace_;
};
}  // namespace search::Game
