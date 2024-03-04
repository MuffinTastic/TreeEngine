#include "system.h"

#include "module.h"

Tree::SystemRegistry::SystemRegistry( System* system, std::string_view name )
	: m_System( system ), m_Name( name )
{
	Module::GetCurrent().RegisterSystem( *this );
}