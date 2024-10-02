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

		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		virtual ESysGroupLoadCode LoadGroupsFrom( std::vector<std::tuple<std::string, ESysGroupTag>> modules ) override;
		virtual void RemoveWithTag( ESysGroupTag tag ) override;

	private:
		void SetSysStates();
		void UnloadUnusedModules();
		void UnloadAllModules();

		std::vector<Platform::PlatformModule*> m_modules;
		std::vector<std::unique_ptr<SysGroup>> m_sysGroups;
	};
}