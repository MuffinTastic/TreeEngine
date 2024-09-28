#pragma once

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
		SAP_GEN ICmdLineSystem* CmdLine();
		SAP_GEN ILogSystem* Log();
		SAP_GEN IWindowSystem* Window();
		IManagedHostSystem* ManagedHost();
		SAP_GEN IEngineSystem* Engine();

		void UpdateFromGroup( SysGroup* module );
		void Reset();
	}
}