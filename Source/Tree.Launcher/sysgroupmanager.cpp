#include "sysgroupmanager.h"

#include <filesystem>

#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/sys.h"


REGISTER_TREE_SYSTEM( SysGroupManager, SYSGROUPMANAGER_NAME )


void Tree::SysGroupManager::Bootstrap()
{
	Platform::PlatformModule* module = Platform::GetExecutableModule();
	
	auto launcherGroup = std::make_unique<SysGroup>( module );
	Sys::UpdateFromGroup( launcherGroup.get() );
	
	auto manager = dynamic_cast<SysGroupManager*>( Sys::SysGroups() );
	manager->m_sysGroups.push_back( std::move( launcherGroup ) );
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
		std::filesystem::path modulePath = enginePath / Platform::UTF8ToPath( *it + MODULE_EXT );

		auto module = Platform::LoadModule( modulePath );
		if ( module == nullptr )
		{
			// We don't have LogSystem yet, so let's just do a platform log.
			Platform::DebugLog( "Couldn't load module '{}'.", Platform::PathToUTF8( modulePath ) );
			return ESYSGROUPLOAD_FAILURE;
		}

		auto sysGroup = std::make_unique<SysGroup>( module );

		// SysGroups don't take ownership of shared libraries, so
		// we keep them around ourselves to free them later.
		m_modules.push_back( module );
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

	for ( auto it = m_modules.begin(); it != m_modules.end(); ++it )
	{
		Platform::PlatformModule* module = *it;
		Platform::UnloadModule( module );
	}

	m_modules.clear();
}
