#pragma once

#include <fights/Common.hpp>

namespace fights
{
enum class ActionType
{
    MOVE,
    PLACE_HORIZONTAL_WALL,
    PLACE_VERTICAL_WALL,
    ROTATE
};

class Action
{
 public:
    virtual ~Action() noexcept = default;

    virtual ActionType GetType() const = 0;
};

namespace Actions
{
class Move : public Action
{
 public:
    enum class Direction
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

 public:
    Move(Direction dir);

    ActionType GetType() const override;

    Direction GetDirection() const;

 private:
    Direction dir_;
};

class PlaceHorizontalWall : public Action
{
 public:
    PlaceHorizontalWall(Point pt);

    ActionType GetType() const override;

    Point GetPosition() const;

 private:
    Point pt_;
};

class PlaceVerticalWall : public Action
{
 public:
    PlaceVerticalWall(Point pt);

    ActionType GetType() const override;

    Point GetPosition() const;

 private:
    Point pt_;
};

class Rotate : public Action
{
 public:
    Rotate(Point pt);

    ActionType GetType() const override;

    Point GetPosition() const;

 private:
    Point pt_;
};
}  // namespace Actions
}  // namespace fights
