#include <selfplay/game.hpp>

#include <cstdio>
#include <numeric>
#include <iostream>

namespace selfplay
{
Game::Game(const search::Config& config)
    : player1_(config)//, player2_(config)
{
}

void Game::RunSingleGame()
{
    search::Game::Environment env;

    while (!env.IsEnd())
    {
        const int turnID = env.GetTurns();

        search::Search::MCTS& currentPla = player1_;
            //(turnID % 2 == 1) ? player1_ : player2_;
        currentPla.DoSearchWithMaxSimulation();

        const auto bestAction = currentPla.GetBestAction();

        // make training data
        states_.emplace_back(search::NN::EnvToState(env));

        search::NN::PolicyVal pi{ 0 };
        currentPla.GetRoot()->ForEachChild(
            [&pi](search::Search::MCTSNode* child) {
                pi[child->action.id] = child->visits;
            });

        const float policySum = std::accumulate(begin(pi), end(pi), 1e-10);
        for (auto& v : pi)
            v = v / policySum;

        pis_.emplace_back(std::move(pi));

        // promote turn
        env.Play(*bestAction.action);
        player1_.Play(bestAction);
        // player2_.Play(bestAction);
    }

    const int blueZ = (env.GetWinner() == fights::Player::BLUE) ? 1 : -1;
    const int redZ = (env.GetWinner() == fights::Player::RED) ? 1 : -1;

    const int totalTurns = states_.size();
    for (int i = 0; i < totalTurns; ++i)
    {
        if (i % 2 == 0)  // Blue turn
        {
            zs_.emplace_back(blueZ);
        }
        else
        {
            zs_.emplace_back(redZ);
        }
    }
}

void Game::DumpToFile(const std::string& filename) const
{
    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp)
    {
        throw std::runtime_error("cannot open file");
    }

    // write metadata
    const int totalTurns = states_.size();
    const int stateSize = search::NN::STATE_SIZE;
    const int actionSpaceSize = search::Game::Environment::ACTION_SPACE_SIZE;
    fwrite(&totalTurns, 4, 1, fp);
    fwrite(&stateSize, 4, 1, fp);
    fwrite(&actionSpaceSize, 4, 1, fp);

    for (int i = 0; i < totalTurns; ++i)
    {
        fwrite(states_[i].data(), 4 * stateSize, 1, fp);
        fwrite(pis_[i].data(), 4 * actionSpaceSize, 1, fp);
        fwrite(&zs_[i], 4, 1, fp);
    }

    fclose(fp);
}
}  // namespace selfplay
