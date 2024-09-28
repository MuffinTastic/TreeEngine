#include "sys.h"

#include "sysgroup.h"

namespace Tree::Sys
{
	static ICmdLineSystem* s_cmdLine = nullptr;
	static ILogSystem* s_log = nullptr;
	static IWindowSystem* s_window = nullptr;
	static IManagedHostSystem* s_managedHost = nullptr;
	static IEngineSystem* s_engine = nullptr;

	ICmdLineSystem* CmdLine() { return s_cmdLine; }
	ILogSystem* Log() { return s_log; }
	IWindowSystem* Window() { return s_window; }
	IManagedHostSystem* ManagedHost() { return s_managedHost; }
	IEngineSystem* Engine() { return s_engine; }

	void UpdateFromGroup( SysGroup* sysGroup )
	{
		if ( !s_cmdLine )
			s_cmdLine = dynamic_cast<ICmdLineSystem*>( sysGroup->GetSystem( CMDLINESYSTEM_NAME ) );
		if ( !s_log )
			s_log = dynamic_cast<ILogSystem*>( sysGroup->GetSystem( LOGSYSTEM_NAME ) );
		if ( !s_window )
			s_window = dynamic_cast<IWindowSystem*>( sysGroup->GetSystem( WINDOWSYSTEM_NAME ) );
		if ( !s_managedHost )
			s_managedHost = dynamic_cast<IManagedHostSystem*>( sysGroup->GetSystem( MANAGEDHOSTSYSTEM_NAME ) );
		if ( !s_engine )
			s_engine = dynamic_cast<IEngineSystem*>( sysGroup->GetSystem( ENGINESYSTEM_NAME ) );
	}

	void Reset()
	{
		s_engine = nullptr;
		s_managedHost = nullptr;
		s_window = nullptr;
		s_log = nullptr;
		s_cmdLine = nullptr;
	}
}