#pragma once

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
}  // namespace PlayerUtils
}  // namespace fights
