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

	namespace Sys
	{
		ISysGroupManager* SysGroups();
		SAP_GEN ICmdLineSystem* CmdLine();
		SAP_GEN ILogSystem* Log();
		SAP_GEN IWindowSystem* Window();
		IManagedHostSystem* ManagedHost();
		SAP_GEN IEngineSystem* Engine();

		void UpdateFromGroup( SysGroup* sysGroup );
		void Reset();
	}
}