#ifndef FAST_ALLOCATOR_FIXED_ALLOCATOR_H_
#define FAST_ALLOCATOR_FIXED_ALLOCATOR_H_

#include <stdexcept>

#include "memory_block.h"

template <size_t CHUNK_SIZE, size_t CHUNKS_COUNT>
class fixed_allocator
{
private:
    using memory_block_t = memory_block<CHUNK_SIZE, CHUNKS_COUNT>;
    using memory_page_t = typename memory_block_t::memory_page_t;
    using fixed_allocator_t = fixed_allocator<CHUNK_SIZE, CHUNKS_COUNT>;

    fixed_allocator()
    {
        create_pool();
    }

    ~fixed_allocator()
    {
        clear_pool();
        destroy_pool();
    }

public:
    fixed_allocator(const fixed_allocator_t&) = delete;

    fixed_allocator_t& operator = (const fixed_allocator_t&) = delete;

    static fixed_allocator_t& instance()
    {
        static fixed_allocator_t instance_;
        return instance_;
    }

public:
    void* allocate(size_t count)
    {
        if (!(0 < count && count <= CHUNK_SIZE))
        {
            throw std::bad_alloc();
        }

        if (free_block_ == nullptr)
        {
            free_block_ = create_block(new memory_page_t());
            append_block(free_block_);

            free_block_->next_free_block = nullptr;
            free_block_->previous_free_block = nullptr;
        }

        auto ptr = free_block_->page->allocate();

        if (free_block_->page->full())
        {
            free_block_ = exclude_from_free_blocks(free_block_);
        }

        return ptr;
    }

    void deallocate(void* ptr)
    {
        if (ptr != nullptr)
        {
            auto page = memory_page_t::get_page(ptr);
            auto block = page->get_owner();

            if (block->page->full())
            {
                free_block_ = include_to_free_blocks(block);
            }

            block->page->deallocate(ptr);

            if (block->page->empty())
            {
                free_block_ = exclude_from_free_blocks(block);
                erase_block(block);
            }
        }
    }

private:
    void create_pool()
    {
        head_ = create_block(nullptr);
        head_->next_block = head_;
        head_->previous_block = head_;
        free_block_ = nullptr;
    }

    void destroy_pool()
    {
        delete head_;
    }

    void clear_pool()
    {
        for (auto block = head_->next_block; block != head_;)
        {
            auto next_block = block->next_block;
            destroy_block(block);
            block = next_block;
        }
    }

    memory_block_t* create_block(memory_page_t* page)
    {
        auto new_block = new memory_block_t();
        new_block->page = page;
        if (page != nullptr)
        {
            new_block->page->set_owner(new_block);
        }
        return new_block;
    }

    void destroy_block(memory_block_t* block)
    {
        delete block->page;
        delete block;
    }

    void append_block(memory_block_t* block)
    {
        if (block != nullptr)
        {
            block->previous_block = head_->previous_block;
            block->next_block = head_;
            head_->previous_block->next_block = block;
            head_->previous_block = block;
        }
    }

    void erase_block(memory_block_t* block)
    {
        if (block->next_block != nullptr)
        {
            block->next_block->previous_block = block->previous_block;
        }
        if (block->previous_block != nullptr)
        {
            block->previous_block->next_block = block->next_block;
        }
        destroy_block(block);
    }

    memory_block_t* exclude_from_free_blocks(memory_block_t* block)
    {
        if (block->previous_free_block != nullptr)
        {
            block->previous_free_block->next_free_block
                = block->next_free_block;
        }
        if (block->next_free_block != nullptr)
        {
            block->next_free_block->previous_free_block
                = block->previous_free_block;
        }

        return block->previous_free_block != nullptr
            ? block->previous_free_block
            : block->next_free_block;
    }

    memory_block_t* include_to_free_blocks(memory_block_t* block)
    {
        if (free_block_ != nullptr)
        {
            block->previous_free_block = free_block_->previous_free_block;
            block->next_free_block = free_block_;
            if (free_block_->previous_free_block != nullptr)
            {
                free_block_->previous_free_block->next_free_block = block;
            }
            free_block_->previous_free_block = block;
        }
        else
        {
            block->next_free_block = nullptr;
            block->previous_free_block = nullptr;
        }

        return block;
    }

private:
    // head of the pool of memory blocks
    memory_block_t* head_;
    // first available free block
    memory_block_t* free_block_;
};

#endif