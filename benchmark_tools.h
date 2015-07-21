#ifndef FAST_ALLOCATOR_BENCHMARK_TOOLS_H_
#define FAST_ALLOCATOR_BENCHMARK_TOOLS_H_

#include <chrono>

// Class to measure the execution time of a callable
template <typename TimeT = std::chrono::milliseconds, 
class ClockT = std::chrono::system_clock>
struct measure
{
    template<typename F, typename ...Args>
    static typename TimeT::rep execution(F func, Args&&... args)
    {
        auto start = ClockT::now();

        func(std::forward<Args>(args)...);

        auto duration = std::chrono::duration_cast<TimeT>(ClockT::now() - start);

        return duration.count();
    }

    template<typename F, typename... Args>
    static TimeT duration(F func, Args&&... args)
    {
        auto start = ClockT::now();

        func(std::forward<Args>(args)...);

        return std::chrono::duration_cast<TimeT>(ClockT::now() - start);
    }
};

#endif