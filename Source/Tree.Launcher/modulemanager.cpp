#include "modulemanager.h"

#include <filesystem>

#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/sys.h"

Tree::ModuleManager& Tree::ModuleManager::Instance()
{
	static ModuleManager s_manager;
	return s_manager;
}

Tree::EModuleLoadCode Tree::ModuleManager::LoadModules( std::vector<std::string> names )
{
	auto enginePath = std::filesystem::path( "Engine" );

	for ( auto it = names.begin(); it != names.end(); it++ )
	{
		std::string filename = *it + SHAREDLIB_EXT;
		std::string relativePath = (enginePath / filename).string();

		auto library = Platform::LoadSharedLibrary( relativePath );
		if ( library == nullptr )
		{
			Platform::DebugLog( "Couldn't load shared library '" + relativePath + "'." );
			return EMODULELOAD_FAILURE;
		}

		auto module = std::make_unique<Module>( library );

		m_sharedLibraries.push_back( library );
		m_modules.push_back( std::move( module ) );
	}

	for ( auto it = m_modules.begin(); it != m_modules.end(); it++ )
	{
		Module* module = it->get();

		for ( auto it2 = m_modules.begin(); it2 != m_modules.end(); it2++ )
		{
			module->UpdateSystems( it2->get() );
		}

		// Update the launcher module with the systems as well
		Sys::UpdateFromModule( module );
	}

	return EMODULELOAD_SUCCESS;
}

void Tree::ModuleManager::UnloadModules()
{
	m_modules.clear();

	for ( auto it = m_sharedLibraries.begin(); it != m_sharedLibraries.end(); it++ )
	{
		Platform::UnloadSharedLibrary( *it );
	}

	m_sharedLibraries.clear();
}
