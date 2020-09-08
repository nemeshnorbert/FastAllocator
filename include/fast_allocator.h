#ifndef FAST_ALLOCATOR_FAST_ALLOCATOR_H_
#define FAST_ALLOCATOR_FAST_ALLOCATOR_H_

#include <cstddef>
#include <memory>
#include <limits>

#include "fixed_allocator.h"

// A custom allocator with several pools to allocate small objects
// Memory for large objects allocates via standard allocator

template<typename T>
class fast_allocator;

template <>
class fast_allocator<void>
{

public:
    using value_type = void;
    using pointer = void*;
    using const_pointer = const void*;
    using void_pointer = void*;
    using const_void_pointer = const void*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propogate_on_container_move_assignment = std::false_type;

    template<typename Other>
    struct rebind
    {
        using other = fast_allocator<Other>;
    };


    fast_allocator()
    {}

    fast_allocator(const fast_allocator<void>& alloc)
    {}

    template <typename U>
    fast_allocator(const fast_allocator<U>& alloc)
    {}

    ~fast_allocator()
    {}

    template <typename U>
    fast_allocator<void>& operator = (const fast_allocator<U>& alloc)
    {
        return *this;
    }
};

template <typename T>
class fast_allocator
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using void_pointer = std::allocator<void>::pointer;
    using const_void_pointer = typename fast_allocator<void>::const_pointer;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propogate_on_container_move_assignment = std::false_type;

    template<typename Other>
    struct rebind
    {
        using other = fast_allocator<Other>;
    };

    fast_allocator()
    {}

    fast_allocator(const fast_allocator<T>& alloc)
    {}

    template<typename U>
    fast_allocator(const fast_allocator<U>& alloc)
    {}

    fast_allocator<T>& operator = (const fast_allocator<T>& other)
    {
        return *this;
    }

    ~fast_allocator()
    {}

    pointer address(reference value) const throw()
    {
        return std::addressof(value);
    }

    const_pointer address(const_reference value) const throw()
    {
        return std::addressof(value);
    }

    pointer allocate(size_type count, const_void_pointer hint = 0)
    {
        size_t bytes_count = sizeof(T)* count;
        pointer ptr;
        if (bytes_count <= 4)
        {
            ptr = static_cast<pointer>(
                fixed_allocator<4, 262144>::instance().allocate(bytes_count));
        }
        else if (bytes_count <= 16)
        {
            ptr = static_cast<pointer>(
                fixed_allocator<16, 65536>::instance().allocate(bytes_count));
        }
        else if (bytes_count <= 64)
        {
            ptr = static_cast<pointer>(
                fixed_allocator<64, 16384>::instance().allocate(bytes_count));
        }
        else if (bytes_count <= 512)
        {
            ptr = static_cast<pointer>(
                fixed_allocator<512, 4096>::instance().allocate(bytes_count));
        }
        else
        {
            ptr = std::allocator<T>().allocate(count, hint);
        }
        return ptr;
    }

    void deallocate(pointer ptr, size_type count)
    {
        size_t bytes_count = sizeof(T)* count;

        if (bytes_count <= 4)
        {
            fixed_allocator<4, 262144>::instance().deallocate(ptr);
        }
        else if (bytes_count <= 16)
        {
            fixed_allocator<16, 65536>::instance().deallocate(ptr);
        }
        else if (bytes_count <= 64)
        {
            fixed_allocator<64, 16384>::instance().deallocate(ptr);
        }
        else if (bytes_count <= 512)
        {
            fixed_allocator<512, 4096>::instance().deallocate(ptr);
        }
        else
        {
            std::allocator<T>().deallocate(ptr, count);
        }
    }

    size_type max_size() const throw()
    {
        return std::numeric_limits<size_t>::max();
    }

    template<typename U, typename ... Args>
    void construct(U* p, Args&& ... args)
    {
        ::new(static_cast<void_pointer>(p)) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p)
    {
        p->~U();
    }
};

template <typename T, typename U>
bool operator == (const fast_allocator<T>& lhs, const fast_allocator<U>& rhs)
{
    return true;
}

template <typename T, typename U>
bool operator != (const fast_allocator<T>& lhs, const fast_allocator<U>& rhs)
{
    return !(lhs == rhs);
}


#endif