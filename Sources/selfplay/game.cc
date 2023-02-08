#include <selfplay/game.hpp>

#include <uuid/uuid.h>
#include <array>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <effolkronium/random.hpp>

namespace selfplay
{
std::string Game::GenerateRandomGameID()
{
    std::array<unsigned char, 16> uuid;
    uuid_generate_random(uuid.data());

    std::stringstream ss;

    const auto print = [&ss](unsigned char val) {
        ss << std::setfill('0') << std::setw(2) << std::hex
           << static_cast<unsigned int>(val);
    };

    print(uuid[0]);
    print(uuid[1]);
    print(uuid[2]);
    print(uuid[3]);
    ss << '-';
    print(uuid[4]);
    print(uuid[5]);
    ss << '-';
    print(uuid[6]);
    print(uuid[7]);
    ss << '-';
    print(uuid[8]);
    print(uuid[9]);
    ss << '-';
    print(uuid[10]);
    print(uuid[11]);
    print(uuid[12]);
    print(uuid[13]);
    print(uuid[14]);
    print(uuid[15]);

    return ss.str();
}

Game::Game(const search::Config& config) : player1_(config), player2_(config)
{
}

void Game::RunSingleGame()
{
    using Random = effolkronium::random_thread_local;

    search::Game::Environment env;

    const int originalSimulations = player1_.GetConfig().search.MaxSimulation;
    const int cuttedSimulations = 50;

    while (!env.IsEnd() && env.GetTurns() < 150)
    {
        const int turnID = env.GetTurns();

        search::Search::MCTS& currentPla =
            (turnID % 2 == 1) ? player1_ : player2_;

        if (Random::get<double>(0, 1) <= 0.25)
        {
            currentPla.GetConfig().search.MaxSimulation = cuttedSimulations;
            currentPla.GetConfig().search.EnableDirichletNoise = false;
            omit_.emplace_back(true);
        }
        else
        {
            currentPla.GetConfig().search.MaxSimulation = originalSimulations;
            currentPla.GetConfig().search.EnableDirichletNoise = true;
            omit_.emplace_back(false);
        }

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
        player2_.Play(bestAction);
    }

    int blueZ = (env.GetWinner() == fights::Player::BLUE) ? 1 : -1;
    int redZ = (env.GetWinner() == fights::Player::RED) ? 1 : -1;

    if (env.GetWinner() == fights::Player::NONE)
    {
        blueZ = 0;
        redZ = 0;
    }

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
        fwrite(states_[i].data(), 4, stateSize, fp);
        fwrite(pis_[i].data(), 4, actionSpaceSize, fp);
        fwrite(&zs_[i], 4, 1, fp);
        fwrite(&omit_[i], 4, 1, fp);
    }

    fclose(fp);
}
}  // namespace selfplay
