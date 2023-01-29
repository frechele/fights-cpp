#pragma once

#include <array>
#include <string>

#include <search/Game/Environment.hpp>

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

    virtual bool Initialize(const std::string& path) = 0;
    virtual void Shutdown() = 0;

    NNOutput Predict(const Game::Environment& env);

 private:
    virtual void predictImpl(const Game::Environment& env, PolicyVal& pol,
                             float& val) = 0;
};
}  // namespace search::NN
