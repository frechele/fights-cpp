#pragma once

#include <string>

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

    virtual std::string ToString() const = 0;
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
        RIGHT,

        // special moves (needs help from opponent)
        L_UP,
        R_UP,
        L_DOWN,
        R_DOWN,
    };

 public:
    Move(Direction dir);

    ActionType GetType() const override;

    std::string ToString() const override;

    Direction GetDirection() const;

 private:
    Direction dir_;
};

class PlaceHorizontalWall : public Action
{
 public:
    PlaceHorizontalWall(Point pt);

    ActionType GetType() const override;

    std::string ToString() const override;

    Point GetPosition() const;

 private:
    Point pt_;
};

class PlaceVerticalWall : public Action
{
 public:
    PlaceVerticalWall(Point pt);

    ActionType GetType() const override;

    std::string ToString() const override;

    Point GetPosition() const;

 private:
    Point pt_;
};

class Rotate : public Action
{
 public:
    Rotate(Point pt);

    ActionType GetType() const override;

    std::string ToString() const override;

    Point GetPosition() const;

 private:
    Point pt_;
};
}  // namespace Actions
}  // namespace fights
