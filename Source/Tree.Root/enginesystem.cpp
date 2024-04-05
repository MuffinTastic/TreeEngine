#include "enginesystem.h"

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include "Tree.NativeCommon/sys.h"


REGISTER_TREE_SYSTEM( EngineSystem, ENGINESYSTEM_NAME )


void Tree::EngineSystem::SetStartupConfig( EngineStartupConfig config )
{
	m_domain = config.domain;
}

Tree::ESystemInitCode Tree::EngineSystem::Startup()
{
	m_logger = Sys::Log()->CreateLogger( "Engine" );

	if ( !Sys::ManagedHost()->TrunkStartup() )
	{
		return ESYSTEMINIT_FAILURE;
	}

	return ESYSTEMINIT_SUCCESS;
}

void Tree::EngineSystem::Shutdown()
{
	Sys::ManagedHost()->TrunkShutdown();

	m_logger->Info( "Engine shutdown" );
}

void Tree::EngineSystem::Run()
{
	m_logger->Info( "Hello {}", 42 );
	// 
	Sys::Log()->Info( Sys::CmdLine()->GetStringOption("bob", "yop") );
}

inline Tree::EDomain Tree::EngineSystem::GetDomain() const
{
	return m_domain;
}

inline bool Tree::EngineSystem::IsGame() const
{
	return ( m_domain & EDOMAIN_GAME ) == EDOMAIN_GAME;
}

inline bool Tree::EngineSystem::IsGameOnly() const
{
	return m_domain == EDOMAIN_GAME;
}

inline bool Tree::EngineSystem::IsEditorOnly() const
{
	return ( m_domain & EDOMAIN_EDITOR_ONLY ) == EDOMAIN_EDITOR_ONLY;
}

inline bool Tree::EngineSystem::IsDedicatedServer() const
{
	return m_domain == EDOMAIN_DEDICATED_SERVER;
}
