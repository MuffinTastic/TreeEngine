#include "memorysystem.h"

#include "Tree.NativeCommon/sys.h"


REGISTER_TREE_SYSTEM( MemorySystem, MEMORYSYSTEM_NAME )


Tree::ESystemInitCode Tree::MemorySystem::Startup()
{
    return ESYSTEMINIT_SUCCESS;
}

void Tree::MemorySystem::Shutdown()
{

}

void* Tree::MemorySystem::Allocate( EMemoryCategory category, size_t size )
{
    void* ret = NULL;

#ifdef WINDOWS
    HANDLE& heap = m_allocHeaps[category];
    
    if ( heap == NULL )
    {
        heap = HeapCreate( 0, 0, 0 );

        if ( !heap ) throw std::bad_alloc();
    }

    ret = HeapAlloc( heap, 0, size );
#else

#endif

    m_allocTrackers[category] += size;
    return ret;
}

void Tree::MemorySystem::Free( EMemoryCategory category, void* p )
{
#ifdef WINDOWS
    HANDLE& heap = m_allocHeaps[category];
    
    HeapFree( heap, 0, p );
#else

#endif
}
