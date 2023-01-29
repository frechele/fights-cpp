#include <search/NN/FakeNet.hpp>

#include <effolkronium/random.hpp>

namespace search::NN
{
void FakeNet::predictImpl([[maybe_unused]] const Game::Environment& env,
                          PolicyVal& pol, float& val)
{
    using Random = effolkronium::random_thread_local;

    for (auto& v : pol)
    {
        v = Random::get<double>(0, 1);
    }

    val = Random::get<double>(-1, 1);
}
}  // namespace search::NN
