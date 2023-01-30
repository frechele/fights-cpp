#pragma once

#include <vector>

#include <fights/Action.hpp>
#include <fights/Game.hpp>

namespace search::Game
{
struct Action final
{
   Action() = default;
   Action(int _id, fights::Action* _action);

   int id{ -1 };  // -1 means invalid action
   fights::Action* action;
};

class Environment : public fights::Game
{
 public:
   static constexpr int ACTION_SPACE_SIZE = 172;

 public:
    Environment();
    ~Environment() noexcept;

    const std::vector<Action>& GetActionSpace() const;
    std::vector<Action> GetValidActions() const;

 private:
    std::vector<Action> actionSpace_;
};
}  // namespace search::Game
