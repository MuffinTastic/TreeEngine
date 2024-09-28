#include "sysgroup.h"

#include <iostream>

#include "sys.h"


Tree::ISystem* __SysGroup_GetSystem( std::string name )
{
    return Tree::SystemRegistry::GetSystem( name );
}

void __SysGroup_UpdateSystems( Tree::SysGroup* module )
{
    Tree::Sys::UpdateFromGroup( module );
}

void __SysGroup_ResetSystems( Tree::SysGroup* module )
{
    Tree::Sys::Reset();
}

Tree::SysGroup::SysGroup( Platform::SharedLibrary* sharedLibrary )
    : m_sharedLibrary( sharedLibrary )
{
    m_getSystem = Platform::GetSharedLibraryFunc<__SysGroup_GetSystemFn>( m_sharedLibrary, SYSGROUP_GETSYSTEM_FUNCNAME );
    m_updateSystems = Platform::GetSharedLibraryFunc<__SysGroup_UpdateSystemsFn>( m_sharedLibrary, SYSGROUP_UPDATESYSTEMS_FUNCNAME );
    m_resetSystems = Platform::GetSharedLibraryFunc<__SysGroup_ResetSystemsFn>( m_sharedLibrary, SYSGROUP_RESETSYSTEMS_FUNCNAME );
}

Tree::ISystem* Tree::SysGroup::GetSystem( std::string name )
{
    return m_getSystem( name );
}

void Tree::SysGroup::UpdateSystems( SysGroup* module )
{
    m_updateSystems( module );
}

void Tree::SysGroup::ResetSystems()
{
    m_resetSystems();
}
