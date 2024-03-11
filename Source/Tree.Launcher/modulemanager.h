#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/module.h"

namespace Tree
{
	enum EModuleLoadCode
	{
		EMODULELOAD_SUCCESS,
		EMODULELOAD_FAILURE
	};

	class ModuleManager
	{
	public:
		static ModuleManager& Instance();

		EModuleLoadCode LoadModules( std::vector<std::string> names );
		void UnloadModules();

	private:

		std::vector<Platform::SharedLibrary*> m_sharedLibraries;
		std::vector<std::unique_ptr<Module>> m_modules;
	};
}