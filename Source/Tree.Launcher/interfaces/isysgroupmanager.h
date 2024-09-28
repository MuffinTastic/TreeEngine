#pragma once

#include <string>
#include <vector>

#include "Tree.NativeCommon/system.h"

#define SYSGROUPMANAGER_NAME "SysGroupManager_v01"

namespace Tree
{
	enum ESysGroupLoadCode
	{
		ESYSGROUPLOAD_SUCCESS,
		ESYSGROUPLOAD_FAILURE
	};

	class ISysGroupManager : public ISystem
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;


		virtual ESysGroupLoadCode LoadGroupsFrom( std::vector<std::string> names ) = 0;
		virtual void UnloadGroups() = 0;
	};
}