#ifndef FAST_ALLOCATOR_MEMORY_PAGE_H_
#define FAST_ALLOCATOR_MEMORY_PAGE_H_

template <size_t CHUNK_SIZE, size_t CHUNKS_COUNT>
class memory_block;

template <size_t CHUNK_SIZE, size_t CHUNKS_COUNT>
class memory_page
{
    static_assert(CHUNK_SIZE != 0 || CHUNKS_COUNT != 0, "wrong configuration!");

public:
    static const size_t STEP = sizeof(void*);
    static const size_t CHUNK_IN_STEPS = (CHUNK_SIZE + STEP - 1) / STEP;
    static const size_t CHUNK_VOLUME = (CHUNK_IN_STEPS + 1) * STEP;

    using memory_page_t = memory_page<CHUNK_SIZE, CHUNKS_COUNT>;
    using memory_block_t = memory_block<CHUNK_SIZE, CHUNKS_COUNT>;

    memory_page()
    {
        chunks_ = new char[CHUNKS_COUNT * CHUNK_VOLUME];
        char** next_free_chunk;
        memory_page_t** parent_page;

        for (size_t chunk_id = 0; chunk_id < CHUNKS_COUNT; ++chunk_id)
        {
            next_free_chunk = reinterpret_cast<char**>(chunks_ +
                CHUNK_VOLUME * chunk_id);
            *next_free_chunk = chunk_id + 1 < CHUNKS_COUNT
                ? chunks_ + CHUNK_VOLUME * (chunk_id + 1)
                : nullptr;

            parent_page = reinterpret_cast<memory_page_t**>(chunks_ +
                CHUNK_VOLUME * (chunk_id + 1) - STEP);
            *parent_page = this;
        }

        free_chunk_ = chunks_;
        used_chunks_ = 0;
    }

    ~memory_page()
    {
        delete[] chunks_;
        free_chunk_ = nullptr;
        used_chunks_ = 0;
    }

    bool full() const
    {
        return used_chunks_ == CHUNKS_COUNT;
    }

    bool empty() const
    {
        return used_chunks_ == 0;
    }

    void* allocate()
    {
        auto here = free_chunk_;
        auto next_free_chunk = reinterpret_cast<char**>(free_chunk_);
        free_chunk_ = *next_free_chunk;
        ++used_chunks_;
        return static_cast<void*>(here);
    }

    void deallocate(void* ptr)
    {
        auto next_free_chunk = reinterpret_cast<char**>(ptr);
        *next_free_chunk = free_chunk_;
        free_chunk_ = static_cast<char*>(ptr);
        --used_chunks_;
    }

    void set_owner(memory_block_t* owner)
    {
        owner_ = owner;
    }

    memory_block_t* get_owner() const
    {
        return owner_;
    }

    static memory_page_t* get_page(void* ptr)
    {
        auto page = reinterpret_cast<char**>(static_cast<char*>(ptr)
            +CHUNK_IN_STEPS * STEP);
        return reinterpret_cast<memory_page_t*>(*page);
    }

private:
    // raw data, each chunk is a data + pointer to parent memory_page
    char* chunks_;
    // pointer to the first available free chunk;
    char* free_chunk_;
    int used_chunks_;
    // memory block this memory_page assigned to
    memory_block_t* owner_;
};

#endif