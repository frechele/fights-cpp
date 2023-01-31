#include <search/Utils/WaitGroup.hpp>

namespace search::Utils
{
void WaitGroup::Add()
{
    add(1);
}

void WaitGroup::Done()
{
    add(-1);
}

void WaitGroup::Wait()
{
    std::unique_lock lock(mutex_);

    cv_.wait(lock, [this] { return counter_ == 0; });
}

void WaitGroup::add(int val)
{
    std::scoped_lock lock(mutex_);

    counter_ += val;

    if (counter_ < 0)
        counter_ = 0;

    if (counter_ == 0)
        cv_.notify_all();
}
}  // namespace search::Utils
