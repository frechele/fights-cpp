#include <fights/Game.hpp>

#include <sstream>

namespace fights
{
Game::Game()
{
    SetPlayerPosition(Player::BLUE, Point((1 + BOARD_SIZE) / 2, BOARD_SIZE));
    SetPlayerPosition(Player::RED, Point((1 + BOARD_SIZE) / 2, 1));
}

Player Game::GetCurrentPlayer() const
{
    return player_;
}

Player Game::GetOpponentPlayer() const
{
    return PlayerUtils::Opponent(player_);
}

int Game::GetRemainWallCount(Player player) const
{
    return remainWallCounts_[static_cast<int>(player)];
}

void Game::SetPlayerPosition(Player player, Point newPosition)
{
    playerPositions_[static_cast<int>(player)] = newPosition;
}

Point Game::GetPlayerPosition(Player player) const
{
    return playerPositions_[static_cast<int>(player)];
}

std::string Game::ToString() const
{
    std::stringstream ss;

    

    return ss.str();
}
}  // namespace fights
