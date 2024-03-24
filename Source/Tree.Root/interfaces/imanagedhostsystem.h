#pragma once

#include "Tree.NativeCommon/system.h"

#define MANAGEDHOSTSYSTEM_NAME "ManagedHostSystem_v01"

namespace Tree
{
	class IManagedHostSystem : public ISystem
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;
	};
}