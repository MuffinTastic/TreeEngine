#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/module.h"

namespace Tree
{
	class ModuleManager
	{
	public:
		static ModuleManager& Instance();

		void LoadModules( std::vector<std::string> names );
		void UnloadModules();

	private:
		std::vector<Platform::SharedLibrary*> m_SharedLibraries;
		std::vector<std::unique_ptr<Module>> m_Modules;
	};
}