#ifndef FAST_ALLOCATOR_BLACK_BOX_TESTS_H_
#define FAST_ALLOCATOR_BLACK_BOX_TESTS_H_

#include "fast_allocator.h"
#include "benchmark_tools.h"

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

#endif