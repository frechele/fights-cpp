#pragma once

namespace fights
{
class Point final
{
 public:
    Point() = default;
    Point(int x, int y);

    Point(const Point&) = default;
    Point(Point&&) = default;

    Point& operator=(const Point&) = default;
    Point& operator=(Point&&) = default;

    int X() const;
    int& X();
    int Y() const;
    int& Y();

 private:
    int x_{ 0 }, y_{ 0 };
};
}
