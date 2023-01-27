#pragma once

#include <array>
#include <string>

#include <fights/Common.hpp>
#include <fights/Player.hpp>

namespace fights
{
constexpr int BOARD_SIZE = 9;
constexpr int WALL_PLACEABLE_SIZE = BOARD_SIZE - 1;
constexpr int ROTATABLE_SIZE = BOARD_SIZE - 3;

constexpr int EXTENDED_BOARD_SIZE = BOARD_SIZE + 2;

constexpr int INITIAL_WALL_COUNT_PER_PLAYER = 10;

class Game final
{
 public:
    Game();
    Game(const Game&) = default;

    //! Returns the color of current player
    Player GetCurrentPlayer() const;
    //! Returns the color of opponent player
    Player GetOpponentPlayer() const;

    //! Returns the number of remain walls of \p player
    int GetRemainWallCount(Player player) const;

    //! \note DO NOT call this method directly.
    void SetPlayerPosition(Player player, Point newPosition);

    //! Returns the position of \p player
    Point GetPlayerPosition(Player player) const;

    //! Visualize the game state
    std::string ToString() const;

 private:
    std::array<Point, 2> playerPositions_;
    std::array<int, 2> remainWallCounts_{ INITIAL_WALL_COUNT_PER_PLAYER,
                                          INITIAL_WALL_COUNT_PER_PLAYER };

    Player player_{ Player::BLUE };
};
}  // namespace fights
