#include <search/Game/Environment.hpp>

namespace search::Game
{
Action::Action(int _id, fights::Action* _action) : id(_id), action(_action)
{
}

Environment::Environment()
{
    int actionID = 0;
    actionSpace_.emplace_back(
        actionID++,
        new fights::Actions::Move(fights::Actions::Move::Direction::UP));
    actionSpace_.emplace_back(
        actionID++,
        new fights::Actions::Move(fights::Actions::Move::Direction::DOWN));
    actionSpace_.emplace_back(
        actionID++,
        new fights::Actions::Move(fights::Actions::Move::Direction::LEFT));
    actionSpace_.emplace_back(
        actionID++,
        new fights::Actions::Move(fights::Actions::Move::Direction::RIGHT));
    actionSpace_.emplace_back(
        actionID++,
        new fights::Actions::Move(fights::Actions::Move::Direction::L_UP));
    actionSpace_.emplace_back(
        actionID++,
        new fights::Actions::Move(fights::Actions::Move::Direction::R_UP));
    actionSpace_.emplace_back(
        actionID++,
        new fights::Actions::Move(fights::Actions::Move::Direction::L_DOWN));
    actionSpace_.emplace_back(
        actionID++,
        new fights::Actions::Move(fights::Actions::Move::Direction::R_DOWN));

    for (int x = 1; x <= fights::WALL_PLACEABLE_SIZE; ++x)
    {
        for (int y = 1; y <= fights::WALL_PLACEABLE_SIZE; ++y)
        {
            actionSpace_.emplace_back(
                actionID++,
                new fights::Actions::PlaceHorizontalWall(fights::Point(x, y)));
            actionSpace_.emplace_back(
                actionID++,
                new fights::Actions::PlaceVerticalWall(fights::Point(x, y)));
        }
    }

    for (int x = 1; x <= fights::ROTATABLE_SIZE; ++x)
    {
        for (int y = 1; y <= fights::ROTATABLE_SIZE; ++y)
        {
            actionSpace_.emplace_back(
                actionID++, new fights::Actions::Rotate(fights::Point(x, y)));
        }
    }
}

Environment::~Environment()
{
    for (const auto act : actionSpace_)
        delete act.action;
}

const std::vector<Action>& Environment::GetActionSpace() const
{
    return actionSpace_;
}

std::vector<Action> Environment::GetValidActions() const
{
    std::vector<Action> result;

    for (const auto act : actionSpace_)
    {
        if (IsValidAction(*act.action))
        {
            result.emplace_back(act);
        }
    }

    return result;
}
}  // namespace search::Game
