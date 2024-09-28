#include "sysgroup.h"

#include <iostream>

#include "sys.h"


Tree::ISystem* __SysGroup_GetSystem( std::string name )
{
    return Tree::SystemRegistry::GetSystem( name );
}

void __SysGroup_UpdateSystems( Tree::SysGroup* sysGroup )
{
    Tree::Sys::UpdateFromGroup( sysGroup );
}

void __SysGroup_ResetSystems( Tree::SysGroup* sysGroup )
{
    Tree::Sys::Reset();
}

Tree::SysGroup::SysGroup( Platform::PlatformModule* module )
    : m_module( module )
{
    m_getSystem = Platform::GetModuleFunc<__SysGroup_GetSystemFn>( m_module, SYSGROUP_GETSYSTEM_FUNCNAME );
    m_updateSystems = Platform::GetModuleFunc<__SysGroup_UpdateSystemsFn>( m_module, SYSGROUP_UPDATESYSTEMS_FUNCNAME );
    m_resetSystems = Platform::GetModuleFunc<__SysGroup_ResetSystemsFn>( m_module, SYSGROUP_RESETSYSTEMS_FUNCNAME );
}

Tree::ISystem* Tree::SysGroup::GetSystem( std::string name )
{
    return m_getSystem( name );
}

void Tree::SysGroup::UpdateSystems( SysGroup* sysGroup )
{
    m_updateSystems( sysGroup );
}

void Tree::SysGroup::ResetSystems()
{
    m_resetSystems();
}
