#include <fights/Game.hpp>

#include <sstream>

namespace fights
{
State::State()
{
    SetPlayerPosition(Player::BLUE, Point((1 + BOARD_SIZE) / 2, BOARD_SIZE));
    SetPlayerPosition(Player::RED, Point((1 + BOARD_SIZE) / 2, 1));
}

Player State::GetCurrentPlayer() const
{
    return player_;
}

Player State::GetOpponentPlayer() const
{
    return PlayerUtils::Opponent(player_);
}

int State::GetRemainWallCount(Player player) const
{
    return remainWallCounts_[static_cast<int>(player)];
}

void State::SetPlayerPosition(Player player, Point newPosition)
{
    playerPositions_[static_cast<int>(player)] = newPosition;
}

Point State::GetPlayerPosition(Player player) const
{
    return playerPositions_[static_cast<int>(player)];
}

std::string State::ToString() const
{
    std::stringstream ss;

    

    return ss.str();
}

const State& Game::GetState() const
{
    return state_;
}
}  // namespace fights
