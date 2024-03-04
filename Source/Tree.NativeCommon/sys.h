#pragma once

#include "Tree.Root/interfaces/ienginesystem.h"

namespace Tree
{
	class Module;

	namespace Sys
	{
		IEngineSystem* Engine();

		void UpdateFromModule( Module* module );
		void Reset();
	}
}