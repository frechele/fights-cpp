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

std::string Move::ToString() const
{
    std::string result = "Move(";

    switch (dir_)
    {
        case Direction::UP:
            result += "UP";
            break;

        case Direction::DOWN:
            result += "DOWN";
            break;

        case Direction::LEFT:
            result += "LEFT";
            break;

        case Direction::RIGHT:
            result += "RIGHT";
            break;

        case Direction::L_UP:
            result += "L_UP";
            break;

        case Direction::R_UP:
            result += "R_UP";
            break;

        case Direction::L_DOWN:
            result += "L_DOWN";
            break;

        case Direction::R_DOWN:
            result += "R_DOWN";
            break;
    }

    return result + ")";
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

std::string PlaceHorizontalWall::ToString() const
{
    using namespace std::string_literals;
    return "PlaceHorizontalWall("s + std::to_string(pt_.X()) + ", " +
           std::to_string(pt_.Y()) + ")";
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

std::string PlaceVerticalWall::ToString() const
{
    using namespace std::string_literals;
    return "PlaceVerticalWall("s + std::to_string(pt_.X()) + ", " +
           std::to_string(pt_.Y()) + ")";
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

std::string Rotate::ToString() const
{
    using namespace std::string_literals;
    return "Rotate("s + std::to_string(pt_.X()) + ", " +
           std::to_string(pt_.Y()) + ")";
}

Point Rotate::GetPosition() const
{
    return pt_;
}
}  // namespace Actions
}  // namespace fights
