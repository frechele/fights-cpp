#include <search/NN/NNBase.hpp>

namespace search::NN
{
NNOutput NNBase::Predict(const Game::Environment& env)
{
    NNOutput result;

    predictImpl(env, result.policy, result.value);

    return result;
}
}
