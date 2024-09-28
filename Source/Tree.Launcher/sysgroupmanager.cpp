#include "sysgroupmanager.h"

#include <filesystem>

#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/sys.h"


REGISTER_TREE_SYSTEM( SysGroupManager, SYSGROUPMANAGER_NAME )


void Tree::SysGroupManager::Bootstrap()
{
}

Tree::ESystemInitCode Tree::SysGroupManager::Startup()
{
	return ESYSTEMINIT_SUCCESS;
}

void Tree::SysGroupManager::Shutdown()
{

}

Tree::ESysGroupLoadCode Tree::SysGroupManager::LoadGroupsFrom( std::vector<std::string> names )
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
			return ESYSGROUPLOAD_FAILURE;
		}

		auto sysGroup = std::make_unique<SysGroup>( library );

		// SysGroups don't take ownership of shared libraries, so
		// we keep them around ourselves to free them later.
		m_sharedLibraries.push_back( library );
		m_sysGroups.push_back( std::move( sysGroup ) );
	}

	//
	// Let all the modules know about each other's systems.
	//

	for ( auto it = m_sysGroups.begin(); it != m_sysGroups.end(); ++it )
	{
		SysGroup* sysGroup = it->get();

		// We don't skip over the current module in the second loop.
		// SysGroups don't set their own system variables, we need to set them here.
		for ( auto it2 = m_sysGroups.begin(); it2 != m_sysGroups.end(); ++it2 )
		{
			sysGroup->UpdateSystems( it2->get() );
		}

		// Update the launcher module with the systems as well
		Sys::UpdateFromGroup( sysGroup );
	}

	return ESYSGROUPLOAD_SUCCESS;
}

void Tree::SysGroupManager::UnloadGroups()
{
	m_sysGroups.clear();

	for ( auto it = m_sharedLibraries.begin(); it != m_sharedLibraries.end(); ++it )
	{
		Platform::UnloadSharedLibrary( *it );
	}

	m_sharedLibraries.clear();
}