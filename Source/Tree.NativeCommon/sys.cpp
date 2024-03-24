#include "sys.h"

#include "module.h"

namespace Tree::Sys
{
	static ICmdLineSystem* s_cmdLine = nullptr;
	static ILogSystem* s_log = nullptr;
	static IManagedHostSystem* s_managedHost = nullptr;
	static IEngineSystem* s_engine = nullptr;

	ICmdLineSystem* CmdLine() { return s_cmdLine; }
	ILogSystem* Log() { return s_log; }
	IManagedHostSystem* ManagedHost() { return s_managedHost; }
	IEngineSystem* Engine() { return s_engine; }

	void UpdateFromModule( Module* module )
	{
		if ( !s_cmdLine )
			s_cmdLine = dynamic_cast<ICmdLineSystem*>( module->GetSystem( CMDLINESYSTEM_NAME ) );
		if ( !s_log )
			s_log = dynamic_cast<ILogSystem*>( module->GetSystem( LOGSYSTEM_NAME ) );
		if ( !s_managedHost )
			s_managedHost = dynamic_cast<IManagedHostSystem*>( module->GetSystem( MANAGEDHOSTSYSTEM_NAME ) );
		if ( !s_engine )
			s_engine = dynamic_cast<IEngineSystem*>( module->GetSystem( ENGINESYSTEM_NAME ) );
	}

	void Reset()
	{
		s_engine = nullptr;
		s_log = nullptr;
		s_managedHost = nullptr;
		s_cmdLine = nullptr;
	}
}