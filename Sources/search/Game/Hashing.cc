#include <search/Game/Hashing.hpp>

#include <effolkronium/random.hpp>

namespace search::Game
{
void Hashing::Init()
{
    using Random = effolkronium::random_static;

    for (auto& hash : PLAYER_POSITION)
    {
        hash = Random::get<std::uint64_t>();
    }

    for (auto& hash : WALLS)
    {
        hash = Random::get<std::uint64_t>();
    }
}

std::uint64_t Hashing::Hash(const Environment& env)
{
    std::uint64_t hash = (env.GetTurns() % 2 == 0) ? HASH_TURN : 0;

    const auto bluePos = env.GetPlayerPosition(fights::Player::BLUE);
    const auto redPos = env.GetPlayerPosition(fights::Player::RED);
    hash ^= PLAYER_POSITION[bluePos.X() + bluePos.Y() * fights::BOARD_SIZE];
    hash ^= PLAYER_POSITION[redPos.X() + redPos.Y() * fights::BOARD_SIZE +
                            fights::BOARD_SIZE * fights::BOARD_SIZE];

    for (int y = 0; y < fights::BOARD_SIZE; ++y)
    {
        for (int x = 0; x < fights::BOARD_SIZE; ++x)
        {
            if (env.IsHorizontalWallPlaced(fights::Point(x + 1, y + 1)))
            {
                hash ^= WALLS[x + y * fights::BOARD_SIZE];
            }

            if (env.IsVerticalWallPlaced(fights::Point(x + 1, y + 1)))
            {
                hash ^= WALLS[x + y * fights::BOARD_SIZE +
                              fights::BOARD_SIZE * fights::BOARD_SIZE];
            }
        }
    }

    return hash;
}
}  // namespace search::Game
