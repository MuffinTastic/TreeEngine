#pragma once

#include "Tree.NativeCommon/system.h"

#define LOGSYSTEM_NAME "LogSystem_v01"

namespace Tree
{
	class ILogSystem : public System
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;
	};
}