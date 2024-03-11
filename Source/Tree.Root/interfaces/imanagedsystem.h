#pragma once

#include "Tree.NativeCommon/system.h"

#define MANAGEDSYSTEM_NAME "ManagedSystem_v01"

namespace Tree
{
	class IManagedSystem : public System
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;
	};
}