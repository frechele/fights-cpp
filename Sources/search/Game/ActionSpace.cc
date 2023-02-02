#include <search/Game/ActionSpace.hpp>

#include <fights/Game.hpp>

namespace search::Game
{
ActionSpace::ActionSpace()
{
    moveActionSpace_.emplace_back(fights::Actions::Move::Direction::UP);
    moveActionSpace_.emplace_back(fights::Actions::Move::Direction::DOWN);
    moveActionSpace_.emplace_back(fights::Actions::Move::Direction::LEFT);
    moveActionSpace_.emplace_back(fights::Actions::Move::Direction::RIGHT);
    moveActionSpace_.emplace_back(fights::Actions::Move::Direction::L_UP);
    moveActionSpace_.emplace_back(fights::Actions::Move::Direction::R_UP);
    moveActionSpace_.emplace_back(fights::Actions::Move::Direction::L_DOWN);
    moveActionSpace_.emplace_back(fights::Actions::Move::Direction::R_DOWN);

    for (int x = 1; x <= fights::WALL_PLACEABLE_SIZE; ++x)
    {
        for (int y = 1; y <= fights::WALL_PLACEABLE_SIZE; ++y)
        {
            placeHorizontalWallActionSpace_.emplace_back(fights::Point(x, y));
            placeVerticalWallActionSpace_.emplace_back(fights::Point(x, y));
        }
    }

    for (int x = 1; x <= fights::ROTATABLE_SIZE; ++x)
    {
        for (int y = 1; y <= fights::ROTATABLE_SIZE; ++y)
        {
            rotateActionSpace_.emplace_back(fights::Point(x, y));
        }
    }

    int actionID = 0;
    for (auto& action : moveActionSpace_)
    {
        totalActionSpace_.emplace_back(actionID++, &action);
    }

    for (auto& action : placeHorizontalWallActionSpace_)
    {
        totalActionSpace_.emplace_back(actionID++, &action);
    }

    for (auto& action : placeVerticalWallActionSpace_)
    {
        totalActionSpace_.emplace_back(actionID++, &action);
    }

    for (auto& action : rotateActionSpace_)
    {
        totalActionSpace_.emplace_back(actionID++, &action);
    }
}

const std::vector<Action>& ActionSpace::GetActionSpace() const
{
    return totalActionSpace_;
}
}  // namespace search::Game
