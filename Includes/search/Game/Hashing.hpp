#pragma once

#include <array>
#include <cstdint>

#include <fights/Game.hpp>
#include <search/Game/Environment.hpp>

namespace search::Game
{
class Hashing
{
 public:
    static void Init();

    static std::uint64_t Hash(const Environment& env);

 private:
    static constexpr std::uint64_t HASH_TURN = 0x1234567890ABCDEF;

    inline static std::array<std::uint64_t,
                             fights::BOARD_SIZE * fights::BOARD_SIZE * 2>
        PLAYER_POSITION;
    inline static std::array<std::uint64_t,
                             fights::BOARD_SIZE * fights::BOARD_SIZE * 2>
        WALLS;
};
}  // namespace search::Game
