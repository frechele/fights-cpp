#pragma once

#include <vector>

#include <fights/Action.hpp>

namespace search::Game
{
struct Action final
{
    Action() = default;
    Action(int _id, fights::Action* _action);

    int id{ -1 };  // -1 means invalid action
    fights::Action* action;
};

class ActionSpace final
{
 public:
    ActionSpace();

    const std::vector<Action>& GetActionSpace() const;

 private:
    std::vector<fights::Actions::Move> moveActionSpace_;
    std::vector<fights::Actions::PlaceHorizontalWall>
        placeHorizontalWallActionSpace_;
    std::vector<fights::Actions::PlaceVerticalWall>
        placeVerticalWallActionSpace_;
    std::vector<fights::Actions::Rotate> rotateActionSpace_;

    std::vector<Action> totalActionSpace_;
};
}  // namespace search::Game
