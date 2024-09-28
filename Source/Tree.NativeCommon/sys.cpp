#include "sys.h"

#include "sysgroup.h"

namespace Tree::Sys
{
	static ISysGroupManager* s_sysGroups = nullptr;
	static ICmdLineSystem* s_cmdLine = nullptr;
	static ILogSystem* s_log = nullptr;
	static IWindowSystem* s_window = nullptr;
	static IManagedHostSystem* s_managedHost = nullptr;
	static IEngineSystem* s_engine = nullptr;

	ISysGroupManager* SysGroups() { return s_sysGroups; }
	ICmdLineSystem* CmdLine() { return s_cmdLine; }
	ILogSystem* Log() { return s_log; }
	IWindowSystem* Window() { return s_window; }
	IManagedHostSystem* ManagedHost() { return s_managedHost; }
	IEngineSystem* Engine() { return s_engine; }

	void UpdateFromGroup( SysGroup* sysGroup )
	{
		{
			ISystem* system = sysGroup->GetSystem( SYSGROUPMANAGER_NAME );
			if ( system )
				s_sysGroups = dynamic_cast<ISysGroupManager*>( system );
		}

		{
			ISystem* system = sysGroup->GetSystem( CMDLINESYSTEM_NAME );
			if ( system )
				s_cmdLine = dynamic_cast<ICmdLineSystem*>( system );
		}

		{
			ISystem* system = sysGroup->GetSystem( LOGSYSTEM_NAME );
			if ( system )
				s_log = dynamic_cast<ILogSystem*>( system );
		}

		{
			ISystem* system = sysGroup->GetSystem( WINDOWSYSTEM_NAME );
			if ( system )
				s_window = dynamic_cast<IWindowSystem*>( system );
		}

		{
			ISystem* system = sysGroup->GetSystem( MANAGEDHOSTSYSTEM_NAME );
			if ( system )
				s_managedHost = dynamic_cast<IManagedHostSystem*>( system );
		}

		{
			ISystem* system = sysGroup->GetSystem( ENGINESYSTEM_NAME );
			if ( system )
				s_engine = dynamic_cast<IEngineSystem*>( system );
		}
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