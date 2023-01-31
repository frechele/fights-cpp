#pragma once

#include <condition_variable>
#include <mutex>

namespace search::Utils
{
class WaitGroup final
{
 public:
    void Add();
    void Done();
    void Wait();

 private:
    void add(int value);

 private:
    int counter_{ 0 };

    std::mutex mutex_;
    std::condition_variable cv_;
};
}
