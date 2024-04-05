#pragma once

#include "Tree.NativeCommon/system.h"

#define WINDOWSYSTEM_NAME "WindowSystem_v01"

namespace Tree
{
	class IWindowSystem : public ISystem
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;
	};
}