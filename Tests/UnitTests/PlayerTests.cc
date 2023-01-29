#include <doctest.h>

#include <fights/Player.hpp>

using namespace fights;

TEST_CASE("[Player] utilities")
{
    CHECK_EQ(PlayerUtils::Opponent(Player::BLUE), Player::RED);
    CHECK_EQ(PlayerUtils::Opponent(Player::RED), Player::BLUE);
}
