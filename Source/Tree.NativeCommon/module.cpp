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
    : m_sharedLibrary( sharedLibrary )
{
    m_getSystem = Platform::GetSharedLibraryFunc<__Module_GetSystemFn>( m_sharedLibrary, MODULE_GETSYSTEM_FUNCNAME );
    m_updateSystems = Platform::GetSharedLibraryFunc<__Module_UpdateSystemsFn>( m_sharedLibrary, MODULE_UPDATESYSTEMS_FUNCNAME );
    m_resetSystems = Platform::GetSharedLibraryFunc<__Module_ResetSystemsFn>( m_sharedLibrary, MODULE_RESETSYSTEMS_FUNCNAME );
}

Tree::System* Tree::Module::GetSystem( std::string name )
{
    return m_getSystem( name );
}

void Tree::Module::UpdateSystems( Module* module )
{
    m_updateSystems( module );
}

void Tree::Module::ResetSystems()
{
    m_resetSystems();
}
