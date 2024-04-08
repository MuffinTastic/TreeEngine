#pragma once

#include <vector>

#include <SDL3/SDL.h>

#include "Tree.Root/interfaces/ilogsystem.h"
#include "interfaces/iwindowsystem.h"

#include "window.h"

#undef CreateWindow

namespace Tree
{
	class WindowSystem : public IWindowSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		inline virtual IWindow* MainWindow() override
		{
			return m_mainWindow.get();
		}

		virtual IWindow* CreateWindow( std::string internalName );

	private:
		ILogger* m_logger;

		std::shared_ptr<IWindow> m_mainWindow;
		std::vector<std::shared_ptr<Window>> m_windows;
	};
}