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

		virtual ESysGroupLoadCode LoadGroupsFrom( std::vector<std::tuple<std::string, ESysGroupTag>> modules );
		virtual void RemoveWithTag( ESysGroupTag tag );

	private:
		void SetSysStates();
		void UnloadUnusedModules();
		void UnloadAllModules();

		std::vector<Platform::PlatformModule*> m_modules;
		std::vector<std::unique_ptr<SysGroup>> m_sysGroups;
	};
}