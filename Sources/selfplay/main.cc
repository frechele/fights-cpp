#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <thread>

#include <search/NN/NNManager.hpp>
#include <search/Game/Hashing.hpp>

#include <selfplay/game.hpp>

search::Config config;
std::atomic<std::uint64_t> numOfGames = 0;
bool stopFlag = false;

void signalHandler(int sig)
{
    if (sig == SIGTERM || sig == SIGKILL)
    {
        stopFlag = true;
    }
}

void workerThread()
{
    using namespace std::string_literals;

    while (!stopFlag)
    {
        selfplay::Game game(config);
        game.RunSingleGame();

        game.DumpToFile("data/"s + selfplay::Game::GenerateRandomGameID());
        ++numOfGames;
    }
}

int main()
{
	config = search::Config::Load("config.json");
    std::cout << config.ToString() << std::endl;

    search::Game::Hashing::Init();

    search::NN::NNManager manager(config);

    selfplay::Game game(config);

    std::vector<std::thread> workers;
    for (int rank = 0; rank < 20; ++rank)
    {
        workers.emplace_back(workerThread);
    }

    while (!stopFlag)
    {
        std::this_thread::sleep_for(std::chrono::minutes(1));

        std::cout << "[LOG] " << numOfGames.load() << " games were created"
                  << std::endl;
    }

    for (auto& worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }

    std::cout << "Total " << numOfGames.load() << " games were created"
              << std::endl;
}
