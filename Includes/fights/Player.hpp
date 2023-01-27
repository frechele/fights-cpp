#pragma once

namespace fights
{
//! type of player
//! blue is the first player.
enum class Player
{
    BLUE,
    RED
};

namespace PlayerUtils
{
constexpr Player Opponent(Player player)
{
    return player == Player::BLUE ? Player::RED : Player::BLUE;
}
}  // namespace PlayerUtils
}  // namespace fights
