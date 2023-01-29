#pragma once

#include <string>

namespace fights
{
//! type of player
//! blue is the first player.
enum class Player
{
    BLUE,
    RED,
    NONE
};

namespace PlayerUtils
{
constexpr Player Opponent(Player player)
{
    if (player == Player::NONE)
        return Player::NONE;

    return player == Player::BLUE ? Player::RED : Player::BLUE;
}

inline std::string ToString(Player player)
{
    switch (player)
    {
    case Player::BLUE:
        return "Blue";
    case Player::RED:
        return "Red";
    case Player::NONE:
        return "None";
    }

    return "Unknown";
}
}  // namespace PlayerUtils
}  // namespace fights
