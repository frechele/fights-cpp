#include <doctest.h>

#include <fights/Game.hpp>
#include <fights/Player.hpp>

using namespace fights;

TEST_CASE("[Game/State] Initialization tests")
{
    State state;

    CHECK_EQ(state.GetCurrentPlayer(), Player::BLUE);
    CHECK_EQ(state.GetOpponentPlayer(), Player::RED);

    CHECK_EQ(state.GetRemainWallCount(Player::BLUE), 10);
    CHECK_EQ(state.GetRemainWallCount(Player::RED), 10);

    const Point bluePosition(5, 9);
    const Point redPosition(5, 1);
    CHECK_EQ(state.GetPlayerPosition(Player::BLUE), bluePosition);
    CHECK_EQ(state.GetPlayerPosition(Player::RED), redPosition);
}

TEST_CASE("[Game/Game] Initialization tests")
{
    Game game;

    CHECK_EQ(game.GetState().GetCurrentPlayer(), Player::BLUE);
    CHECK_EQ(game.GetState().GetOpponentPlayer(), Player::RED);

    CHECK_EQ(game.GetState().GetRemainWallCount(Player::BLUE), 10);
    CHECK_EQ(game.GetState().GetRemainWallCount(Player::RED), 10);

    const Point bluePosition(5, 9);
    const Point redPosition(5, 1);
    CHECK_EQ(game.GetState().GetPlayerPosition(Player::BLUE), bluePosition);
    CHECK_EQ(game.GetState().GetPlayerPosition(Player::RED), redPosition);
}
