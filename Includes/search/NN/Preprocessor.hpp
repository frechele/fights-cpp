#pragma once

#include <array>

#include <fights/Game.hpp>
#include <search/Game/Environment.hpp>

namespace search::NN
{
constexpr int STATE_DIM =
    fights::INITIAL_WALL_COUNT_PER_PLAYER * 2 + 2 +
    2 + 1;  // wall counts (per player) + player position + wall positions + color
constexpr int STATE_SIZE = STATE_DIM * fights::BOARD_SIZE * fights::BOARD_SIZE;

using Tensor = std::array<float, STATE_SIZE>;

Tensor EnvToState(const Game::Environment& env);
}  // namespace search::NN
