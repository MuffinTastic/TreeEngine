#include "module.h"

#include <iostream>

Tree::Module& Tree::Module::GetCurrent()
{
    static Module s_CurrentModule;
    return s_CurrentModule;
}

void Tree::Module::RegisterSystem( SystemRegistry& registry )
{
    m_Systems[registry.m_Name] = registry.m_System;
}

Tree::System* Tree::Module::GetSystem( std::string name )
{
    auto result = m_Systems.find( name );
    
    if ( result != m_Systems.end() )
        return result->second;

    return nullptr;
}


Tree::Module& Tree::GetModuleFromSharedLibrary( Platform::SharedLibrary* sharedLibrary )
{
    void* ptr = Platform::GetSharedLibraryFunc( sharedLibrary, GETMODULE_FUNCNAME );
    __GetModuleFunc getModule = reinterpret_cast<__GetModuleFunc>( ptr );
    return getModule();
}

Tree::Module& __SharedLibrary_GetModule()
{
    return Tree::Module::GetCurrent();
}