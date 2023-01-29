#include <search/Game/Environment.hpp>

namespace search
{
Environment::Environment()
{
    actionSpace_.emplace_back(
        new fights::Actions::Move(fights::Actions::Move::Direction::UP));
    actionSpace_.emplace_back(
        new fights::Actions::Move(fights::Actions::Move::Direction::DOWN));
    actionSpace_.emplace_back(
        new fights::Actions::Move(fights::Actions::Move::Direction::LEFT));
    actionSpace_.emplace_back(
        new fights::Actions::Move(fights::Actions::Move::Direction::RIGHT));
    actionSpace_.emplace_back(
        new fights::Actions::Move(fights::Actions::Move::Direction::L_UP));
    actionSpace_.emplace_back(
        new fights::Actions::Move(fights::Actions::Move::Direction::R_UP));
    actionSpace_.emplace_back(
        new fights::Actions::Move(fights::Actions::Move::Direction::L_DOWN));
    actionSpace_.emplace_back(
        new fights::Actions::Move(fights::Actions::Move::Direction::R_DOWN));

    for (int x = 1; x <= fights::WALL_PLACEABLE_SIZE; ++x)
    {
        for (int y = 1; y <= fights::WALL_PLACEABLE_SIZE; ++y)
        {
            actionSpace_.emplace_back(
                new fights::Actions::PlaceHorizontalWall(fights::Point(x, y)));
            actionSpace_.emplace_back(
                new fights::Actions::PlaceVerticalWall(fights::Point(x, y)));
        }
    }

    for (int x = 1; x <= fights::ROTATABLE_SIZE; ++x)
    {
        for (int y = 1; y <= fights::ROTATABLE_SIZE; ++y)
        {
            actionSpace_.emplace_back(
                new fights::Actions::Rotate(fights::Point(x, y)));
        }
    }
}

Environment::~Environment()
{
    for (const auto act : actionSpace_)
        delete act;
}

std::vector<fights::Action*> Environment::GetActionSpace() const
{
    return actionSpace_;
}

std::vector<fights::Action*> Environment::GetValidActions() const
{
    std::vector<fights::Action*> result;

    for (const auto act : actionSpace_)
    {
        if (IsValidAction(*act))
        {
            result.emplace_back(act);
        }
    }

    return result;
}
}  // namespace search
