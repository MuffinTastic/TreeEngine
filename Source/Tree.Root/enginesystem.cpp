

#include "interfaces/ienginesystem.h"

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include "Tree.NativeCommon/sys.h"
#include "Tree.NativeCommon/domain.h"

#include "interfaces/ilogsystem.h"

namespace Tree
{
	class EngineSystem : public IEngineSystem
	{
	public:
		virtual void SetStartupConfig( EngineStartupConfig config ) override;
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		void Run();

		virtual inline bool IsClient() const override;
		virtual inline bool IsClientOnly() const override;
		virtual inline bool IsEditorOnly() const override;
		virtual inline bool IsDedicatedServer() const override;

	private:
		std::shared_ptr<ILogger> m_logger;
		EDomain m_domain = EDOMAIN_UNINITIALIZED;
	};
}


REGISTER_TREE_SYSTEM( EngineSystem, ENGINESYSTEM_NAME )


void Tree::EngineSystem::SetStartupConfig( EngineStartupConfig config )
{
	m_domain = config.domain;
}

Tree::ESystemInitCode Tree::EngineSystem::Startup()
{
	m_logger = Sys::Log()->CreateLogger( "Engine" );

    return ESYSTEMINIT_SUCCESS;
}

void Tree::EngineSystem::Shutdown()
{
	m_logger->Info( "Engine shutdown" );
}

void Tree::EngineSystem::Run()
{
	m_logger->Info( "Hello {}", 42 );
	// 
	Sys::Log()->Info( Sys::CmdLine()->GetStringOption("bob", "yop") );
}

inline bool Tree::EngineSystem::IsClient() const
{
	return ( m_domain & EDOMAIN_CLIENT ) == EDOMAIN_CLIENT;
}

inline bool Tree::EngineSystem::IsClientOnly() const
{
	return m_domain == EDOMAIN_CLIENT;
}

inline bool Tree::EngineSystem::IsEditorOnly() const
{
	return ( m_domain & EDOMAIN_EDITOR_ONLY ) == EDOMAIN_EDITOR_ONLY;
}

inline bool Tree::EngineSystem::IsDedicatedServer() const
{
	return m_domain == EDOMAIN_DEDICATED_SERVER;
}
