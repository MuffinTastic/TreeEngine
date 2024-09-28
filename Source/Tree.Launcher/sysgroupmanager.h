#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/sysgroup.h"

#include "interfaces/isysgroupmanager.h"

namespace Tree
{
	class SysGroupManager : public ISysGroupManager
	{
	public:
		static void Bootstrap();

		virtual ESystemInitCode Startup();
		virtual void Shutdown();


		virtual ESysGroupLoadCode LoadGroupsFrom( std::vector<std::string> names );
		virtual void UnloadGroups();

	private:

		std::vector<Platform::SharedLibrary*> m_sharedLibraries;
		std::vector<std::unique_ptr<SysGroup>> m_modules;
	};
}