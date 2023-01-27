#include <fights/Common.hpp>

namespace fights
{
Point::Point(int x, int y) : x_(x), y_(y)
{
}

int Point::X() const
{
    return x_;
}

int& Point::X()
{
    return x_;
}

int Point::Y() const
{
    return y_;
}

int& Point::Y()
{
    return y_;
}
}
