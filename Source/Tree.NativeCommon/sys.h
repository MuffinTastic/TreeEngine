#pragma once

#include "Tree.Root/interfaces/icmdlinesystem.h"
#include "Tree.Root/interfaces/ilogsystem.h"
#include "Tree.Root/interfaces/imanagedhostsystem.h"
#include "Tree.Root/interfaces/ienginesystem.h"

namespace Tree
{
	class Module;

	namespace Sys
	{
		SAP_GEN ICmdLineSystem* CmdLine();
		SAP_GEN ILogSystem* Log();
		IManagedHostSystem* ManagedHost();
		SAP_GEN IEngineSystem* Engine();

		void UpdateFromModule( Module* module );
		void Reset();
	}
}