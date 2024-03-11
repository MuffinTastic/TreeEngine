#include "modulemanager.h"

#include <filesystem>

#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/sys.h"

Tree::ModuleManager& Tree::ModuleManager::Instance()
{
	static ModuleManager s_Manager;
	return s_Manager;
}

void Tree::ModuleManager::LoadModules( std::vector<std::string> names )
{
	if ( m_Modules.size() != 0 )
	{
		Platform::ShowError( "ModuleManager::LoadModules called twice" );
		Platform::FatalExit();
	}

	auto enginePath = std::filesystem::path( "Engine" );

	for ( auto it = names.begin(); it != names.end(); it++ )
	{
		std::string filename = *it + SHAREDLIB_EXT;
		auto relativePath = enginePath / filename;

		auto library = Platform::LoadSharedLibrary( relativePath.string() );
		auto module = std::make_unique<Module>( library );

		m_SharedLibraries.push_back( library );
		m_Modules.push_back( std::move( module ) );
	}

	for ( auto it = m_Modules.begin(); it != m_Modules.end(); it++ )
	{
		Module* module = it->get();

		for ( auto it2 = m_Modules.begin(); it2 != m_Modules.end(); it2++ )
		{
			module->UpdateSystems( it2->get() );
		}

		// Update the launcher module with the systems as well
		Sys::UpdateFromModule( module );
	}
}

void Tree::ModuleManager::UnloadModules()
{
	m_Modules.clear();

	for ( auto it = m_SharedLibraries.begin(); it != m_SharedLibraries.end(); it++ )
	{
		Platform::UnloadSharedLibrary( *it );
	}

	m_SharedLibraries.clear();
}
