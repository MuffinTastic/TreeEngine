#include "system.h"

#include "module.h"


Tree::SystemRegistry::SystemRegistry( System* system, std::string_view name )
	: m_System( system ), m_Name( name )
{
	SystemRegistry::Register( *this );
}

std::unordered_map<std::string, Tree::System*>& Tree::SystemRegistry::GetSystems()
{
    static std::unordered_map<std::string, Tree::System*> s_Systems;
    return s_Systems;
}

void Tree::SystemRegistry::Register( SystemRegistry& registry )
{
    GetSystems()[registry.m_Name] = registry.m_System;
}

Tree::System* Tree::SystemRegistry::GetSystem( std::string name )
{
    auto result = GetSystems().find( name );

    if ( result != GetSystems().end() )
        return result->second;

    return nullptr;
}