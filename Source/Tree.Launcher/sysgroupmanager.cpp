#include "sysgroupmanager.h"

#include <filesystem>

#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/sys.h"


REGISTER_TREE_SYSTEM( SysGroupManager, SYSGROUPMANAGER_NAME )


void Tree::SysGroupManager::Bootstrap()
{
	Platform::PlatformModule* module = Platform::GetExecutableModule();

	auto launcherGroup = std::make_unique<SysGroup>( module, ESYSGROUPTAG_CORE );

	SysState tempState{};
	tempState.UpdateFromGroup( launcherGroup.get() );
	
	auto manager = dynamic_cast<SysGroupManager*>( tempState.sysGroups );
	manager->m_sysGroups.push_back( std::move( launcherGroup ) );

	// This sets the SysState of the local module
	manager->SetSysStates();
}

Tree::ESystemInitCode Tree::SysGroupManager::Startup()
{
	return ESYSTEMINIT_SUCCESS;
}

void Tree::SysGroupManager::Shutdown()
{
	UnloadAllModules();
}

Tree::ESysGroupLoadCode Tree::SysGroupManager::LoadGroupsFrom( std::vector<std::tuple<std::string, ESysGroupTag>> modules )
{
	std::filesystem::path enginePath = Platform::GetEngineDirectoryPath();

	//
	// Load all the modules from disk.
	//

	for ( auto it = modules.begin(); it != modules.end(); ++it )
	{
		std::string name = std::get<0>( *it );
		ESysGroupTag tag = std::get<1>( *it );

		std::filesystem::path modulePath = enginePath / Platform::UTF8ToPath( name + MODULE_EXT );

		auto module = Platform::LoadModule( modulePath );
		if ( module == nullptr )
		{
			// We don't have LogSystem yet, so let's just do a platform log.
			Platform::DebugLog( "Couldn't load module '{}'.", Platform::PathToUTF8( modulePath ) );
			return ESYSGROUPLOAD_FAILURE;
		}

		auto sysGroup = std::make_unique<SysGroup>( module, tag );

		// SysGroups don't take ownership of shared libraries, so
		// we keep them around ourselves to free them later.
		m_modules.push_back( module );
		m_sysGroups.push_back( std::move( sysGroup ) );
	}

	//
	// Let all the modules know about each other's systems.
	//

	SetSysStates();

	return ESYSGROUPLOAD_SUCCESS;
}

void Tree::SysGroupManager::SetSysStates()
{
	// First generate the SysState from all loaded SysGroups

	SysState state{};

	for ( auto it = m_sysGroups.begin(); it != m_sysGroups.end(); ++it )
	{
		SysGroup* sysGroup = it->get();

		state.UpdateFromGroup( sysGroup );
	}

	// Next, send it to all the groups

	for ( auto it = m_sysGroups.begin(); it != m_sysGroups.end(); ++it )
	{
		SysGroup* sysGroup = it->get();

		sysGroup->SetSysState( state );
	}
}

void Tree::SysGroupManager::RemoveWithTag( ESysGroupTag tag )
{
	for ( auto it = m_sysGroups.begin(); it != m_sysGroups.end(); ++it )
	{
		SysGroup* sysGroup = it->get();
		if ( sysGroup->GetTag() == tag )
		{
			it = m_sysGroups.erase( it );
		}
	}

	SetSysStates();

	UnloadUnusedModules();
}

void Tree::SysGroupManager::UnloadUnusedModules()
{
	for ( auto it = m_modules.begin(); it != m_modules.end(); ++it )
	{
		Platform::PlatformModule* module = *it;
		bool foundDependency = false;

		for ( auto it2 = m_sysGroups.begin(); it2 != m_sysGroups.end(); ++it2 )
		{
			SysGroup* sysGroup = it2->get();
			if ( sysGroup->GetModule() == module )
			{
				foundDependency = true;
				break;
			}
		}

		if ( !foundDependency )
		{
			Platform::UnloadModule( module );
			it = m_modules.erase( it );
		}
	}
}

void Tree::SysGroupManager::UnloadAllModules()
{
	m_sysGroups.clear();

	for ( auto it = m_modules.begin(); it != m_modules.end(); ++it )
	{
		Platform::PlatformModule* module = *it;
		Platform::UnloadModule( module );
	}

	m_modules.clear();
}
