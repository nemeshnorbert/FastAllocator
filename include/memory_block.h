#ifndef FAST_ALLOCATOR_MEMORY_BLOCK_H_
#define FAST_ALLOCATOR_MEMORY_BLOCK_H_

#include "memory_page.h"

template <size_t CHUNK_SIZE, size_t CHUNKS_COUNT>
class memory_block
{
public:
    using memory_block_t = memory_block<CHUNK_SIZE, CHUNKS_COUNT>;
    using memory_page_t = memory_page<CHUNK_SIZE, CHUNKS_COUNT>;

    memory_block()
        : page(nullptr)
        , previous_block(nullptr)
        , next_block(nullptr)
        , previous_free_block(nullptr)
        , next_free_block(nullptr)
    {}

public:
    //data storage
    memory_page_t* page;
    //used to link blocks created by allocator
    memory_block_t* previous_block;
    memory_block_t* next_block;
    //used to represent allocation deallocation logic for blocks
    memory_block_t* previous_free_block;
    memory_block_t* next_free_block;
};

#endif