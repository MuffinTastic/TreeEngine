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
		ICmdLineSystem* CmdLine();
		ILogSystem* Log();
		IManagedHostSystem* ManagedHost();
		IEngineSystem* Engine();

		void UpdateFromModule( Module* module );
		void Reset();
	}
}