#include <search/NN/NNBase.hpp>

namespace search::NN
{
std::vector<NNOutput> NNBase::Predict(const std::vector<Game::Environment>& env)
{
    const int batchSize = env.size();

    std::vector<NNOutput> result(batchSize);

    std::vector<PolicyVal> policies(batchSize);
    std::vector<float> values(batchSize);
    predictImpl(env, policies, values);

    for (int batchID = 0; batchID < batchSize; ++batchID)
    {
        result[batchID].policy = std::move(policies[batchID]);
        result[batchID].value = values[batchID];
    }

    return result;
}
}  // namespace search::NN
