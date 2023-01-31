#pragma once

#include <atomic>

namespace search::Utils
{
template <typename T>
void AtomicAdd(std::atomic<T>& t, T value)
{
    T oldValue = t.load();

    while (!t.compare_exchange_weak(oldValue, oldValue + value))
        ;
}
}  // namespace search::Utils
