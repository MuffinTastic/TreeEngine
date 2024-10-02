#pragma once

#include <memory>

#include "Tree.NativeCommon/memory.h"


template<typename T>
using TreeUniquePtr = std::unique_ptr<T, TreeHeapDeleter<T>>;

template<typename T>
using TreeSharedPtr = std::shared_ptr<T>;

template<typename T, typename... Args>
TreeUniquePtr<T> tree_make_unique( Args&&... args )
{
    TreeHeapAlloc<T> alloc;
    T* ptr = alloc.allocate( 1 );
    std::allocator_traits<TreeHeapAlloc<T>>::construct( alloc, ptr, std::forward<Args>( args )... );
    return TreeUniquePtr<T>( ptr, TreeHeapDeleter<T>{alloc} );
}

template<typename T, typename... Args>
TreeSharedPtr<T> tree_make_shared( Args&&... args )
{
    return std::allocate_shared<T>( TreeHeapAlloc<T>(), std::forward<Args>( args )... );
}