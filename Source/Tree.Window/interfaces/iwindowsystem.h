#pragma once

#include "Tree.NativeCommon/defs.h"
#include "Tree.NativeCommon/system.h"

#include "iwindow.h"

#define WINDOWSYSTEM_NAME "WindowSystem_v01"

namespace Tree
{
	class IWindowSystem : public ISystem
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;

		SAP_GEN virtual IWindow* MainWindow() = 0;
	};
}