#include <doctest.h>

#include <fights/Game.hpp>
#include <fights/Player.hpp>

using namespace fights;

TEST_CASE("[Game] Initialization tests")
{
    Game game;

    CHECK_EQ(game.GetCurrentPlayer(), Player::BLUE);
    CHECK_EQ(game.GetOpponentPlayer(), Player::RED);

    CHECK_EQ(game.GetRemainWallCount(Player::BLUE), 10);
    CHECK_EQ(game.GetRemainWallCount(Player::RED), 10);

    const Point bluePosition(5, 9);
    const Point redPosition(5, 1);
    CHECK_EQ(game.GetPlayerPosition(Player::BLUE), bluePosition);
    CHECK_EQ(game.GetPlayerPosition(Player::RED), redPosition);
}
