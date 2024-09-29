#include "sysgroup.h"

#include <iostream>

#include "sys.h"


Tree::ISystem* __SysGroup_GetSystem( std::string name )
{
    return Tree::SystemRegistry::GetSystem( name );
}

void __SysGroup_SetSysState( Tree::SysState sysState )
{
    Tree::Sys::SetState( sysState );
}

void __SysGroup_ClearSysState()
{
    Tree::Sys::ClearState();
}

Tree::SysGroup::SysGroup( Platform::PlatformModule* module, ESysGroupTag tag )
    : m_module( module ), m_tag( tag )
{
    m_getSystem = Platform::GetModuleFunc<__SysGroup_GetSystemFn>( m_module, SYSGROUP_GETSYSTEM_FUNCNAME );
    m_setSysState = Platform::GetModuleFunc<__SysGroup_SetSysStateFn>( m_module, SYSGROUP_SETSYSSTATE_FUNCNAME );
    m_clearSysState = Platform::GetModuleFunc<__SysGroup_ClearSysStateFn>( m_module, SYSGROUP_CLEARSYSSTATE_FUNCNAME );
}

Tree::Platform::PlatformModule* Tree::SysGroup::GetModule() const noexcept
{
    return m_module;
}

Tree::ESysGroupTag Tree::SysGroup::GetTag() const noexcept
{
    return m_tag;
}

Tree::ISystem* Tree::SysGroup::GetSystem( std::string name )
{
    return m_getSystem( name );
}

void Tree::SysGroup::SetSysState( SysState sysState )
{
    m_setSysState( sysState );
}

void Tree::SysGroup::ClearSysState()
{
    m_clearSysState();
}
