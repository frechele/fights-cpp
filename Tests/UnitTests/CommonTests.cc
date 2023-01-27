#include <doctest.h>

#include <fights/Common.hpp>

using namespace fights;

TEST_CASE("[Common] Point constructions")
{
    Point p1;
    CHECK_EQ(p1.X(), 0);
    CHECK_EQ(p1.Y(), 0);

    Point p2(3, 4);
    CHECK_EQ(p2.X(), 3);
    CHECK_EQ(p2.Y(), 4);
}
