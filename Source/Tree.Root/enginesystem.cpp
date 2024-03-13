

#include "interfaces/ienginesystem.h"

#include <vector>
#include <string>
#include <iostream>

#include "Tree.NativeCommon/sys.h"
#include "Tree.NativeCommon/domain.h"

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
		EDomain m_domain;
	};
}


REGISTER_TREE_SYSTEM( EngineSystem, ENGINESYSTEM_NAME )


void Tree::EngineSystem::SetStartupConfig( EngineStartupConfig config )
{
	m_domain = config.domain;
}

Tree::ESystemInitCode Tree::EngineSystem::Startup()
{


    return ESYSTEMINIT_SUCCESS;
}

void Tree::EngineSystem::Shutdown()
{
    std::cout << "Engine shutdown" << std::endl;
}

void Tree::EngineSystem::Run()
{
	auto args = Sys::CmdLine()->GetArguments();

	for ( auto it = args.begin(); it != args.end(); ++it )
	{
		std::cout << "'" << *it << "'" << std::endl;
	}

	if ( IsClient() )
	{
		std::cout << "IsClient" << std::endl;
	}
	if ( IsClientOnly() )
	{
		std::cout << "IsClientOnly" << std::endl;
	}
	if ( IsEditorOnly() )
	{
		std::cout << "IsEditorOnly" << std::endl;
	}
	if ( IsDedicatedServer() )
	{
		std::cout << "IsDedicatedServer" << std::endl;
	}
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
