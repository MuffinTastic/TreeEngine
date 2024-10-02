#pragma once

#include "Tree.Root/interfaces/imemorysystem.h"
#include "sys.h"

#include <limits>
#include <new>

#undef max

template <typename T, Tree::EMemoryCategory E = Tree::EMEMCATEGORY_GENERAL>
class TreeHeapAlloc
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind
    {
        using other = TreeHeapAlloc<U, E>;
    };

    TreeHeapAlloc() noexcept
    {

    }

    TreeHeapAlloc( const TreeHeapAlloc& other ) noexcept {}

    template <typename U>
    TreeHeapAlloc( const TreeHeapAlloc<U, E>& other ) noexcept {}

    ~TreeHeapAlloc()
    {

    }

    pointer allocate( size_type n )
    {
        if ( n > max_size() )
        {
            throw std::bad_alloc();
        }

        void* p = Tree::Sys::Memory()->Allocate( E, n );

        return static_cast<pointer>( p );
    }

    void deallocate( pointer p, size_type ) noexcept
    {
        Tree::Sys::Memory()->Free( E, p );
    }

    size_type max_size() const noexcept
    {
        return std::numeric_limits<size_type>::max() / sizeof( T );
    }

    template <typename U, typename... Args>
    void construct( U* p, Args&&... args )
    {
        new( p ) U( std::forward<Args>( args )... );
    }

    template <typename U>
    void destroy( U* p )
    {
        p->~U();
    }

    bool operator==( const TreeHeapAlloc& other ) const noexcept
    {
        return true;
    }

    bool operator!=( const TreeHeapAlloc& other ) const noexcept
    {
        return !( *this == other );
    }
};

template<typename T>
struct TreeHeapDeleter
{
    TreeHeapAlloc<T> alloc;

    void operator()( T* ptr )
    {
        if ( ptr )
        {
            std::allocator_traits<TreeHeapAlloc<T>>::destroy( alloc, ptr );
            alloc.deallocate( ptr, 1 );
        }
    }
};