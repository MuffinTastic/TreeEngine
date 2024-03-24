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
	std::filesystem::path enginePath = Platform::GetEngineDirectoryPath();

	//
	// Load all the modules from disk.
	//

	for ( auto it = names.begin(); it != names.end(); ++it )
	{
		std::filesystem::path modulePath = enginePath / Platform::UTF8ToPath( *it + SHAREDLIB_EXT );

		auto library = Platform::LoadSharedLibrary( modulePath );
		if ( library == nullptr )
		{
			// We don't have LogSystem yet, so let's just do a platform log.
			Platform::DebugLog( "Couldn't load shared library '{}'.", Platform::PathToUTF8( modulePath ) );
			return EMODULELOAD_FAILURE;
		}

		auto module = std::make_unique<Module>( library );

		// Modules don't take ownership of shared libraries, so
		// we keep them around ourselves to free them later.
		m_sharedLibraries.push_back( library );
		m_modules.push_back( std::move( module ) );
	}

	//
	// Let all the modules know about each other's systems.
	//

	for ( auto it = m_modules.begin(); it != m_modules.end(); ++it )
	{
		Module* module = it->get();

		// We don't skip over the current module in the second loop.
		// Modules don't set their own system variables, we need to set them here.
		for ( auto it2 = m_modules.begin(); it2 != m_modules.end(); ++it2 )
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

	for ( auto it = m_sharedLibraries.begin(); it != m_sharedLibraries.end(); ++it )
	{
		Platform::UnloadSharedLibrary( *it );
	}

	m_sharedLibraries.clear();
}
