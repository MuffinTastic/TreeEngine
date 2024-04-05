#pragma once

#include "Tree.Root/interfaces/ilogsystem.h"

#include "interfaces/iwindowsystem.h"

namespace Tree
{
	class WindowSystem : public IWindowSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

	private:
		std::shared_ptr<ILogger> m_logger;
	};
}