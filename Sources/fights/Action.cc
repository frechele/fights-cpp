#include <fights/Action.hpp>

namespace fights
{
namespace Actions
{
Move::Move(Move::Direction dir) : dir_(dir)
{
}

ActionType Move::GetType() const
{
    return ActionType::MOVE;
}

Move::Direction Move::GetDirection() const
{
    return dir_;
}

PlaceHorizontalWall::PlaceHorizontalWall(Point pt) : pt_(pt)
{
}

ActionType PlaceHorizontalWall::GetType() const
{
    return ActionType::PLACE_HORIZONTAL_WALL;
}

Point PlaceHorizontalWall::GetPosition() const
{
    return pt_;
}

PlaceVerticalWall::PlaceVerticalWall(Point pt) : pt_(pt)
{
}

ActionType PlaceVerticalWall::GetType() const
{
    return ActionType::PLACE_VERTICAL_WALL;
}

Point PlaceVerticalWall::GetPosition() const
{
    return pt_;
}

Rotate::Rotate(Point pt) : pt_(pt)
{
}

ActionType Rotate::GetType() const
{
    return ActionType::ROTATE;
}

Point Rotate::GetPosition() const
{
    return pt_;
}
}  // namespace Actions
}  // namespace fights
