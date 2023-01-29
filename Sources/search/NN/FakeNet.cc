#include <search/NN/FakeNet.hpp>

#include <effolkronium/random.hpp>

namespace search::NN
{
void FakeNet::Initialize(int rank, const std::string& filename)
{
}

void FakeNet::Shutdown()
{
}

void FakeNet::predictImpl(const std::vector<Game::Environment>& env,
                          std::vector<PolicyVal>& pols,
                          std::vector<float>& vals)
{
    const int batchSize = env.size();

    using Random = effolkronium::random_thread_local;

    for (auto& pol : pols)
    {
        for (auto& v : pol)
        {
            v = Random::get<double>(0, 1);
        }
    }

    for (auto& val : vals)
    {
        val = Random::get<double>(-1, 1);
    }
}
}  // namespace search::NN
