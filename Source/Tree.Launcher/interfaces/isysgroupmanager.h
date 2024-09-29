#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "Tree.NativeCommon/system.h"

#define SYSGROUPMANAGER_NAME "SysGroupManager_v01"

namespace Tree
{
	enum ESysGroupTag;

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


		virtual ESysGroupLoadCode LoadGroupsFrom( std::vector<std::tuple<std::string, ESysGroupTag>> modules ) = 0;
		virtual void RemoveWithTag( ESysGroupTag tag ) = 0;
	};
}