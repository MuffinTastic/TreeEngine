#pragma once

#include "Tree.Launcher/interfaces/isysgroupmanager.h"
#include "Tree.Root/interfaces/icmdlinesystem.h"
#include "Tree.Root/interfaces/ilogsystem.h"
#include "Tree.Root/interfaces/imanagedhostsystem.h"
#include "Tree.Root/interfaces/ienginesystem.h"
#include "Tree.Window/interfaces/iwindowsystem.h"

// Sys is a convenient way to access all of the Systems available to the engine.

namespace Tree
{
	class SysGroup;

	struct SysState
	{
		ISysGroupManager* sysGroups = nullptr;
		ICmdLineSystem* cmdLine = nullptr;
		ILogSystem* log = nullptr;
		IWindowSystem* window = nullptr;
		IManagedHostSystem* managedHost = nullptr;
		IEngineSystem* engine = nullptr;

		void UpdateFromGroup( SysGroup* sysGroup );
	};

	namespace Sys
	{
		ISysGroupManager* SysGroups();
		SAP_GEN ICmdLineSystem* CmdLine();
		SAP_GEN ILogSystem* Log();
		SAP_GEN IWindowSystem* Window();
		IManagedHostSystem* ManagedHost();
		SAP_GEN IEngineSystem* Engine();

		void SetState( SysState state );
		void ClearState();
	}
}