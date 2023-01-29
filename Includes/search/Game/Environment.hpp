#pragma once

#include <vector>

#include <fights/Action.hpp>
#include <fights/Game.hpp>

namespace search
{
class Environment : public fights::Game
{
 public:
    Environment();
    ~Environment() noexcept;

    std::vector<fights::Action*> GetActionSpace() const;
    std::vector<fights::Action*> GetValidActions() const;

 private:
    std::vector<fights::Action*> actionSpace_;
};
}  // namespace search
