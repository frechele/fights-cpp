#pragma once

#include <vector>

#include <search/Game/Environment.hpp>
#include <search/NN/NNBase.hpp>
#include <search/NN/Preprocessor.hpp>
#include <search/Search/MCTS.hpp>

namespace selfplay
{
class Game
{
 public:
    Game(const search::Config& config);

    void RunSingleGame();

    void DumpToFile(const std::string& filename) const;

 private:
    search::Search::MCTS player1_;
    // search::Search::MCTS player2_;

    std::vector<search::NN::Tensor> states_;
    std::vector<search::NN::PolicyVal> pis_;
    std::vector<float> zs_;
};
}  // namespace selfplay
