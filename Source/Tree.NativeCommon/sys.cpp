#include "sys.h"

#include "sysgroup.h"

void Tree::SysState::UpdateFromGroup( SysGroup* sysGroup )
{
	{
		ISystem* system = sysGroup->GetSystem( SYSGROUPMANAGER_NAME );
		if ( system )
			sysGroups = dynamic_cast<ISysGroupManager*>( system );
	}

	{
		ISystem* system = sysGroup->GetSystem( CMDLINESYSTEM_NAME );
		if ( system )
			cmdLine = dynamic_cast<ICmdLineSystem*>( system );
	}

	{
		ISystem* system = sysGroup->GetSystem( LOGSYSTEM_NAME );
		if ( system )
			log = dynamic_cast<ILogSystem*>( system );
	}

	{
		ISystem* system = sysGroup->GetSystem( WINDOWSYSTEM_NAME );
		if ( system )
			window = dynamic_cast<IWindowSystem*>( system );
	}

	{
		ISystem* system = sysGroup->GetSystem( MANAGEDHOSTSYSTEM_NAME );
		if ( system )
			managedHost = dynamic_cast<IManagedHostSystem*>( system );
	}

	{
		ISystem* system = sysGroup->GetSystem( ENGINESYSTEM_NAME );
		if ( system )
			engine = dynamic_cast<IEngineSystem*>( system );
	}
}

namespace Tree::Sys
{
	static SysState s_state{};

	ISysGroupManager* SysGroups() { return s_state.sysGroups; }
	ICmdLineSystem* CmdLine() { return s_state.cmdLine; }
	ILogSystem* Log() { return s_state.log; }
	IWindowSystem* Window() { return s_state.window; }
	IManagedHostSystem* ManagedHost() { return s_state.managedHost; }
	IEngineSystem* Engine() { return s_state.engine; }

	void SetState( SysState state )
	{
		s_state = state;
	}

	void ClearState()
	{
		s_state = {};
	}
}