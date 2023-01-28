#include <doctest.h>

#include <fights/Game.hpp>
#include <fights/Player.hpp>

using namespace fights;

TEST_CASE("[Game/WallBoard] Initialization tests")
{
    WallBoard board;

    for (int i = 0; i <= BOARD_SIZE; ++i)
    {
        for (int j = 0; j <= BOARD_SIZE + 1; ++j)
        {
            CHECK_FALSE(board.IsHorizontalWallPlaced(j, i));
            CHECK_FALSE(board.IsVerticalWallPlaced(i, j));
        }
    }
}

TEST_CASE("[Game/WallBoard] Place tests")
{
    WallBoard board;
    board.PlaceHorizontalWall(3, 4);
    board.PlaceHorizontalWall(2, 1);
    board.PlaceVerticalWall(6, 5);
    board.PlaceVerticalWall(8, 3);

    for (int x = 0; x <= BOARD_SIZE + 1; ++x)
    {
        for (int y = 0; y <= BOARD_SIZE; ++y)
        {
            const bool needPlaced =
                ((x == 3) && (y == 4)) || ((x == 4) && (y == 4)) ||
                ((x == 2) && (y == 1)) || ((x == 3) && (y == 1));
            CHECK_EQ(board.IsHorizontalWallPlaced(x, y), needPlaced);
        }
    }

    for (int x = 0; x <= BOARD_SIZE; ++x)
    {
        for (int y = 0; y <= BOARD_SIZE + 1; ++y)
        {
            const bool needPlaced =
                ((x == 6) && (y == 5)) || ((x == 6) && (y == 6)) ||
                ((x == 8) && (y == 3)) || ((x == 8) && (y == 4));
            CHECK_EQ(board.IsVerticalWallPlaced(x, y), needPlaced);
        }
    }
}

TEST_CASE("[Game/WallBoard] Rotation tests (basic)")
{
    WallBoard board;
    board.PlaceVerticalWall(4, 5);
    board.PlaceVerticalWall(4, 7);
    board.PlaceHorizontalWall(5, 2);
    board.PlaceVerticalWall(6, 3);
    board.PlaceVerticalWall(6, 5);

    board.Rotate(5, 3);

    // Check horizontal walls
    for (int x = 0; x <= BOARD_SIZE + 1; ++x)
    {
        for (int y = 0; y <= BOARD_SIZE; ++y)
        {
            const bool needPlaced =
                ((x == 5) && (y == 2)) || ((x == 6) && (y == 2)) ||
                ((x == 5) && (y == 4)) || ((x == 6) && (y == 4)) ||
                ((x == 7) && (y == 4)) || ((x == 8) && (y == 4));
            CHECK_MESSAGE(board.IsHorizontalWallPlaced(x, y) == needPlaced,
                          "x = " << x << ", y = " << y);
        }
    }

    // Check vertical walls
    for (int x = 0; x <= BOARD_SIZE; ++x)
    {
        for (int y = 0; y <= BOARD_SIZE + 1; ++y)
        {
            const bool needPlaced =
                ((x == 4) && (y == 7)) || ((x == 4) && (y == 8)) ||
                ((x == 8) && (y == 3)) || ((x == 8) && (y == 4));
            CHECK_MESSAGE(board.IsVerticalWallPlaced(x, y) == needPlaced,
                          "x = " << x << ", y = " << y);
        }
    }
}

TEST_CASE("[Game/WallBoard] Rotation tests (mid split)")
{
    WallBoard board;
    board.PlaceVerticalWall(4, 5);
    board.PlaceVerticalWall(4, 7);
    board.PlaceHorizontalWall(5, 2);
    board.PlaceVerticalWall(6, 3);
    board.PlaceVerticalWall(6, 5);

    board.Rotate(5, 4);

    // Check horizontal walls
    for (int x = 0; x <= BOARD_SIZE + 1; ++x)
    {
        for (int y = 0; y <= BOARD_SIZE; ++y)
        {
            const bool needPlaced =
                ((x == 5) && (y == 2)) || ((x == 6) && (y == 2)) ||
                ((x == 5) && (y == 3)) || ((x == 6) && (y == 3)) ||
                ((x == 7) && (y == 3)) || ((x == 6) && (y == 5)) ||
                ((x == 7) && (y == 5)) || ((x == 8) && (y == 5));
            CHECK_MESSAGE(board.IsHorizontalWallPlaced(x, y) == needPlaced,
                          "x = " << x << ", y = " << y);
        }
    }

    // Check vertical walls
    for (int x = 0; x <= BOARD_SIZE; ++x)
    {
        for (int y = 0; y <= BOARD_SIZE + 1; ++y)
        {
            const bool needPlaced =
                ((x == 4) && (y == 8)) || ((x == 6) && (y == 3));
            CHECK_MESSAGE(board.IsVerticalWallPlaced(x, y) == needPlaced,
                          "x = " << x << ", y = " << y);
        }
    }
}

TEST_CASE("[Game/WallBoard] Rotation tests (go to border)")
{
    WallBoard board;
    board.PlaceVerticalWall(4, 5);
    board.PlaceVerticalWall(4, 7);
    board.PlaceHorizontalWall(5, 2);
    board.PlaceVerticalWall(6, 3);
    board.PlaceVerticalWall(6, 5);

    board.Rotate(6, 3);

    // Check horizontal walls
    for (int x = 0; x <= BOARD_SIZE + 1; ++x)
    {
        for (int y = 0; y <= BOARD_SIZE; ++y)
        {
            const bool needPlaced =
                ((x == 5) && (y == 2)) || ((x == 6) && (y == 3)) ||
                ((x == 7) && (y == 3)) || ((x == 8) && (y == 3)) ||
                ((x == 9) && (y == 3));
            CHECK_MESSAGE(board.IsHorizontalWallPlaced(x, y) == needPlaced,
                          "x = " << x << ", y = " << y);
        }
    }

    // Check vertical walls
    for (int x = 0; x <= BOARD_SIZE; ++x)
    {
        for (int y = 0; y <= BOARD_SIZE + 1; ++y)
        {
            const bool needPlaced =
                ((x == 4) && (y == 5)) || ((x == 4) && (y == 6)) ||
                ((x == 4) && (y == 7)) || ((x == 4) && (y == 8));
            CHECK_MESSAGE(board.IsVerticalWallPlaced(x, y) == needPlaced,
                          "x = " << x << ", y = " << y);
        }
    }
}

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
