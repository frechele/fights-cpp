#include <iostream>

#include <search/NN/NNManager.hpp>

#include <selfplay/game.hpp>

int main()
{
    search::Config config;
    config.search.NumWorkers = 0;

    search::NN::NNManager manager(config);

    selfplay::Game game(config);

    game.RunSingleGame();
    game.DumpToFile("test.bin");
}
