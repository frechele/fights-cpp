#pragma once

#include <array>
#include <string>
#include <vector>

#include <fights/Action.hpp>
#include <fights/Common.hpp>
#include <fights/Player.hpp>

namespace fights
{
constexpr int BOARD_SIZE = 9;
constexpr int WALL_PLACEABLE_SIZE = BOARD_SIZE - 1;
constexpr int ROTATABLE_SIZE = BOARD_SIZE - 3;

constexpr int EXTENDED_BOARD_SIZE = BOARD_SIZE + 2;

constexpr int INITIAL_WALL_COUNT_PER_PLAYER = 10;

class WallBoard final
{
 public:
    bool IsHorizontalWallPlaced(int x, int y) const;
    bool IsVerticalWallPlaced(int x, int y) const;
    bool IsIntersection(int x, int y) const;

    void PlaceHorizontalWall(int x, int y);
    void PlaceVerticalWall(int x, int y);
    void Rotate(int x, int y);

    bool CheckReachability(Point playerPos, int target) const;

    std::string ToString() const;

 private:
    static constexpr int pointToHorizontalWallIndex(int x, int y)
    {
        return x * (BOARD_SIZE + 1) + y;
    }

    static constexpr int pointToVerticalWallIndex(int x, int y)
    {
        return x + y * (BOARD_SIZE + 1);
    }

    static constexpr int pointToBoardIndex(int x, int y)
    {
      return x + y * EXTENDED_BOARD_SIZE;
    }

    static constexpr int pointToIntersectionIndex(int x, int y)
    {
        return x + y * (BOARD_SIZE + 1);
    }

    void clearBoundary();

 private:
    static constexpr unsigned char HORIZONTAL_WALL = 0b01;
    static constexpr unsigned char VERTICAL_WALL = 0b10;

    std::array<unsigned char, (BOARD_SIZE + 1) * (BOARD_SIZE + 2)> walls_{ 0 };
    std::array<bool, (BOARD_SIZE + 1) * (BOARD_SIZE + 1)> intersection_{ 0 };
};

class Game
{
 public:
    Game(bool flipped = false);
    Game(const Game&) = default;
    Game(Game&&) = default;

    Game& operator=(const Game&) = default;
    Game& operator=(Game&&) = default;

    virtual ~Game() noexcept = default;

    //! Returns the color of current player
    Player GetCurrentPlayer() const;
    //! Returns the color of opponent player
    Player GetOpponentPlayer() const;

    //! Returns the number of remain walls of \p player
    int GetRemainWallCount(Player player) const;

    //! Returns the position of \p player
    Point GetPlayerPosition(Player player) const;

    //! Returns the target line of \p player
    int GetPlayerTarget(Player player) const;

    //! Returns whether there is horizontal wall at \p pos
    bool IsHorizontalWallPlaced(Point pos) const;

    //! Returns whether there is vertical wall at \p pos
    bool IsVerticalWallPlaced(Point pos) const;

    //! Returns whether there is intersection at \p pos
    bool IsIntersection(Point pos) const;

    //! Returns the winner
    Player GetWinner() const;

    //! Returns wheter the game is end or not
    bool IsEnd() const;

    //! Visualize the game state
    std::string ToString() const;

    //! Checks \p action is valid for \p player.
    //! \param player if Player::NONE, use the current player color
    bool IsValidAction(const Action& action,
                       Player player = Player::NONE) const;

    //! Play the game
    //! \param player if Player::NONE, use the current player color
    void Play(const Action& action, Player player = Player::NONE);

    //! Returns current turn number
    int GetTurns() const;

    //! Returns the action history
    const std::vector<Action*> GetHistory() const;

 private:
    bool isValidMove(const Actions::Move& action, Player player) const;
    bool isValidPlaceHorizontalWall(const Actions::PlaceHorizontalWall& action,
                                    Player player) const;
    bool isValidPlaceVerticalWall(const Actions::PlaceVerticalWall& action,
                                  Player player) const;
    bool isValidRotate(const Actions::Rotate& action, Player player) const;

    void playMove(const Actions::Move& action, Player player);
    void playPlaceHorizontalWall(const Actions::PlaceHorizontalWall& action, Player player);
    void playPlaceVerticalWall(const Actions::PlaceVerticalWall& action, Player player);
    void playRotate(const Actions::Rotate& action, Player player);

    void setPlayerPosition(Player player, Point newPosition);
    void setPlayerTarget(Player player, int newTarget);

    int& getRemainWallCount(Player player);

 private:
    int turns_{ 1 };
    std::array<Point, 2> playerPositions_;
    std::array<int, 2> playerTargets_;
    std::array<int, 2> remainWallCounts_{ INITIAL_WALL_COUNT_PER_PLAYER,
                                          INITIAL_WALL_COUNT_PER_PLAYER };
    std::vector<Action*> history_;

    WallBoard wallBoard_;

    Player player_{ Player::BLUE };
    bool flipped_;
};
}  // namespace fights
