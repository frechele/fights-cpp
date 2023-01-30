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

    // player position ([0] for current, [1] for opponent)
    const auto currentPos = env.GetPlayerPosition(currentPla);
    const auto opponentPos = env.GetPlayerPosition(opponentPla);
    ret[idx(0, currentPos.X(), currentPos.Y())] = 1;
    ret[idx(1, opponentPos.X(), opponentPos.Y())] = 1;

    // wall positions ([2] for horizontal, [3] for vertical)
    for (int y = 1; y <= fights::BOARD_SIZE; ++y)
    {
        for (int x = 1; x <= fights::BOARD_SIZE; ++x)
        {
            const fights::Point pt(x, y);

            if (env.IsHorizontalWallPlaced(pt))
                ret[idx(2, x, y)] = 1;

            if (env.IsVerticalWallPlaced(pt))
                ret[idx(3, x, y)] = 1;
        }
    }

    // remaining wall counts ([4-14] for current, [15-25] for opponent)
    const auto currentWalls = env.GetRemainWallCount(currentPla) - 1;
    const auto opponentWalls = env.GetRemainWallCount(opponentPla) - 1;
    if (currentWalls >= 0)
    {
        std::fill(begin(ret) + idx(4 + currentWalls, 1, 1),
                  begin(ret) + idx(5 + currentWalls, 1, 1), 1);
    }
    if (opponentWalls >= 0)
    {
        std::fill(begin(ret) + idx(15 + opponentWalls, 1, 1),
                  begin(ret) + idx(16 + opponentWalls, 1, 1), 1);
    }

    // color ([26])
    std::fill(begin(ret) + idx(26, 1, 1), begin(ret) + idx(27, 1, 1),
              currentPla == fights::Player::BLUE);

    return ret;
}
}  // namespace search::NN
