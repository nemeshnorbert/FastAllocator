// FastAllocator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <map>
#include <iostream>
#include <numeric>

#include "black_box_tests.h"

// Comparison of standard std::allocator and 
// hand made fast_allocator (with storage for small objects)

int main()
{
    size_t entries_count = 1000000;
    std::cout << "Fast allocator test: " 
        << measure<>::execution(test_allocator<fast_allocator>, entries_count) 
        << " milliseconds\n";
    std::cout << "Standard allocator test: " 
        << measure<>::execution(test_allocator<std::allocator>, entries_count) 
        << " milliseconds\n";
    std::cin.get();
    return 0;
}

