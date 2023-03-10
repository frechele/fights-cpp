#pragma once

#include <array>
#include <string>
#include <vector>

#include <search/NN/Preprocessor.hpp>

namespace search::NN
{
using PolicyVal = std::array<float, Game::Environment::ACTION_SPACE_SIZE>;

struct NNOutput final
{
    PolicyVal policy;
    float value;
};

class NNBase
{
 public:
    virtual ~NNBase() noexcept = default;

    virtual void Initialize(int rank, const std::string& path) = 0;
    virtual void Shutdown() = 0;

    std::vector<NNOutput> Predict(const std::vector<Tensor>& state);

 private:
    virtual void predictImpl(const std::vector<Tensor>& state,
                             std::vector<PolicyVal>& pol,
                             std::vector<float>& val) = 0;
};
}  // namespace search::NN
