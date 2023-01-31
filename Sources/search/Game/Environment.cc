#include <search/Game/Environment.hpp>

namespace search::Game
{
Action::Action(int _id, fights::Action* _action) : id(_id), action(_action)
{
}

const std::vector<Action>& Environment::GetActionSpace() const
{
    return actionSpace_.GetActionSpace();
}

std::vector<Action> Environment::GetValidActions() const
{
    std::vector<Action> result;

    for (const auto act : GetActionSpace())
    {
        if (IsValidAction(*act.action))
        {
            result.emplace_back(act);
        }
    }

    return result;
}
}  // namespace search::Game
