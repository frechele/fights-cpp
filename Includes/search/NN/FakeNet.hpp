#pragma once

#include <search/NN/NNBase.hpp>

namespace search::NN
{
class FakeNet : public NNBase
{
    void predictImpl(const Game::Environment& env, PolicyVal& pol, float& val) override;
};
}
