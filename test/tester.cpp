#include <string>
#include <map>
#include <iostream>
#include <chrono>

#include "fast_allocator.h"

template <template <typename T> class AllocatorType>
size_t test_allocator(size_t entries_count)
{
    std::map<int, std::string, std::less<int>,
        AllocatorType<std::pair<int, std::string>>> int_to_digits;
    for (size_t entry_id = 0; entry_id < entries_count; ++entry_id)
    {
        int_to_digits[entry_id] = std::to_string(entry_id);
    }

    size_t total_size = 0;
    for (const auto& entry : int_to_digits)
    {
        total_size += sizeof(entry);
    }
    return total_size;
}

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

bool run_test(std::ostream& out, size_t entries_count)
{
    try
    {
        out << "Fast allocator test:     "
            << measure<>::execution(test_allocator<fast_allocator>, entries_count)
            << " milliseconds\n";
        out << "Standard allocator test: "
            << measure<>::execution(test_allocator<std::allocator>, entries_count)
            << " milliseconds\n";
    }
    catch (const std::exception& exc)
    {
        out << "Test failed. Reason: " << exc.what() << '\n';
        return false;
    }
    out << "Test passed\n";
    return true;
}

int main()
{
    // Comparison of the standard std::allocator and
    // hand made fast_allocator (with storage for small objects)

    for (int attempt = 0; attempt < 10; ++attempt)
    {
        if(!run_test(std::cout, 1000000))
        {
            return 1;
        }
    }
    return 0;

}
