#include <search/NN/Preprocessor.hpp>

#include <algorithm>

namespace search::NN
{
constexpr int idx(int c, int x, int y)
{
    return (x - 1) + (y - 1) * fights::BOARD_SIZE +
           c * (fights::BOARD_SIZE * fights::BOARD_SIZE);
}

Tensor EnvToState(const Game::Environment& env)
{
    Tensor ret{ 0 };

    const auto currentPla = env.GetCurrentPlayer();
    const auto opponentPla = env.GetOpponentPlayer();

    int chan = 0;
    // player position
    const auto currentPos = env.GetPlayerPosition(currentPla);
    const auto opponentPos = env.GetPlayerPosition(opponentPla);
    ret[idx(chan, currentPos.X(), currentPos.Y())] = 1;
    ret[idx(chan + 1, opponentPos.X(), opponentPos.Y())] = 1;
    chan += 2;

    // wall positions
    for (int y = 1; y <= fights::BOARD_SIZE; ++y)
    {
        for (int x = 1; x <= fights::BOARD_SIZE; ++x)
        {
            const fights::Point pt(x, y);

            if (env.IsHorizontalWallPlaced(pt))
                ret[idx(chan, x, y)] = 1;

            if (env.IsVerticalWallPlaced(pt))
                ret[idx(chan + 1, x, y)] = 1;
        }
    }
    chan += 2;

    // remaining wall counts ([4-14] for current, [15-25] for opponent)
    const auto currentWalls = env.GetRemainWallCount(currentPla) - 1;
    const auto opponentWalls = env.GetRemainWallCount(opponentPla) - 1;
    if (currentWalls >= 0)
    {
        std::fill(begin(ret) + idx(chan + currentWalls, 1, 1),
                  begin(ret) + idx(chan + 1 + currentWalls, 1, 1), 1);
    }
    if (opponentWalls >= 0)
    {
        std::fill(
            begin(ret) + idx(chan + fights::INITIAL_WALL_COUNT_PER_PLAYER +
                                 opponentWalls,
                             1, 1),
            begin(ret) + idx(chan + fights::INITIAL_WALL_COUNT_PER_PLAYER + 1 +
                                 opponentWalls,
                             1, 1),
            1);
    }
    chan += 2 * fights::INITIAL_WALL_COUNT_PER_PLAYER;

    // color ([26])
    std::fill(begin(ret) + idx(chan, 1, 1), begin(ret) + idx(chan + 1, 1, 1),
              currentPla == fights::Player::BLUE);
    chan += 1;

    return ret;
}
}  // namespace search::NN
