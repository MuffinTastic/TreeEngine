#include "system.h"

#include "sysgroup.h"


Tree::SystemRegistry::SystemRegistry( ISystem* system, std::string_view name )
	: m_system( system ), m_name( name )
{
	SystemRegistry::Register( *this );
}

std::unordered_map<std::string, Tree::ISystem*>& Tree::SystemRegistry::GetSystems()
{
    static std::unordered_map<std::string, Tree::ISystem*> s_Systems;
    return s_Systems;
}

void Tree::SystemRegistry::Register( SystemRegistry& registry )
{
    GetSystems()[registry.m_name] = registry.m_system;
}

Tree::ISystem* Tree::SystemRegistry::GetSystem( std::string name )
{
    auto result = GetSystems().find( name );

    if ( result != GetSystems().end() )
        return result->second;

    return nullptr;
}