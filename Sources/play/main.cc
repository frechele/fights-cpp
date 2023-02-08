#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include <search/Config.hpp>
#include <search/Game/Environment.hpp>
#include <search/Game/Hashing.hpp>
#include <search/NN/NNManager.hpp>
#include <search/Search/MCTS.hpp>

void playGame(const search::Config& config, bool comFirst);

int main(int argc, char** argv)
{
    search::Config config;

    if (argc > 1)
    {
        config = search::Config::Load(argv[1]);
    }

    search::Game::Hashing::Init();

    search::NN::NNManager nnManager(config);

    std::cout << "COM first? (y/n): ";
    std::string response;
    std::getline(std::cin, response);

    const bool comFirst = response == "y";
    playGame(config, comFirst);
}

search::Game::Action parseAction(const search::Game::Environment& env,
                                 const std::string& command)
{
    std::stringstream ss(command);

    std::string cmd;
    ss >> cmd;

    const auto& actionSpace = env.GetActionSpace();

    if (cmd == "UP")
    {
        return actionSpace[0];
    }
    else if (cmd == "DOWN")
    {
        return actionSpace[1];
    }
    else if (cmd == "LEFT")
    {
        return actionSpace[2];
    }
    else if (cmd == "RIGHT")
    {
        return actionSpace[3];
    }
    else if (cmd == "L_UP")
    {
        return actionSpace[4];
    }
    else if (cmd == "R_UP")
    {
        return actionSpace[5];
    }
    else if (cmd == "L_DOWN")
    {
        return actionSpace[6];
    }
    else if (cmd == "R_DOWN")
    {
        return actionSpace[7];
    }
    else if (cmd == "HORIZONTAL")
    {
        int x, y;
        ss >> x >> y;

        return actionSpace[8 + (y - 1) + (x - 1) * fights::WALL_PLACEABLE_SIZE];
    }
    else if (cmd == "VERTICAL")
    {
        int x, y;
        ss >> x >> y;

        return actionSpace[8 +
                           fights::WALL_PLACEABLE_SIZE *
                               fights::WALL_PLACEABLE_SIZE +
                           (y - 1) + (x - 1) * fights::WALL_PLACEABLE_SIZE];
    }
    else if (cmd == "ROTATE")
    {
        int x, y;
        ss >> x >> y;

        return actionSpace[8 +
                           fights::WALL_PLACEABLE_SIZE *
                               fights::WALL_PLACEABLE_SIZE * 2 +
                           (y - 1) + (x - 1) * fights::ROTATABLE_SIZE];
    }

    search::Game::Action nullAction;
    return nullAction;
}

void playGame(const search::Config& config, bool comFirst)
{
    search::Game::Environment env;
    search::Search::MCTS engine(config);

    while (!env.IsEnd())
    {
        const int turnID = env.GetTurns();

        search::Game::Action action;

        std::cout << "[turn " << turnID << "]" << std::endl;

        if (turnID % 2 == comFirst)
        {
            engine.DoSearchWithMaxSimulation();
            action = engine.GetBestAction();

            engine.DumpStats();

            std::cout << "COM: ";
        }
        else
        {
            while (true)
            {
                std::cout << env.ToString() << "\n> ";
                std::cout.flush();

                std::string response;
                std::getline(std::cin, response);

                action = parseAction(env, response);

                if (action.id != -1)
                {
                    break;
                }
            }

            std::cout << "YOU: ";
        }

        engine.Play(action);
        env.Play(*action.action);

        std::cout << action.action->ToString() << "\n\n"
                  << env.ToString() << std::endl;
    }
}
