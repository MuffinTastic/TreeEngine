#include "module.h"

#include <iostream>

#include "sys.h"


Tree::System* __Module_GetSystem( std::string name )
{
    return Tree::SystemRegistry::GetSystem( name );
}

void __Module_UpdateSystems( Tree::Module* module )
{
    Tree::Sys::UpdateFromModule( module );
}

void __Module_ResetSystems( Tree::Module* module )
{
    Tree::Sys::Reset();
}

Tree::Module::Module( Platform::SharedLibrary* sharedLibrary )
    : m_SharedLibrary( sharedLibrary )
{
    m_GetSystem = Platform::GetSharedLibraryFunc<__Module_GetSystemFn>( m_SharedLibrary, MODULE_GETSYSTEM_FUNCNAME );
    m_UpdateSystems = Platform::GetSharedLibraryFunc<__Module_UpdateSystemsFn>( m_SharedLibrary, MODULE_UPDATESYSTEMS_FUNCNAME );
    m_ResetSystems = Platform::GetSharedLibraryFunc<__Module_ResetSystemsFn>( m_SharedLibrary, MODULE_RESETSYSTEMS_FUNCNAME );
}

Tree::System* Tree::Module::GetSystem( std::string name )
{
    return m_GetSystem( name );
}

void Tree::Module::UpdateSystems( Module* module )
{
    m_UpdateSystems( module );
}

void Tree::Module::ResetSystems()
{
    m_ResetSystems();
}
