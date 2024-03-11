#pragma once

#include "interfaces/ienginesystem.h"

#include <vector>
#include <string>

#include "Tree.NativeCommon/domain.h"

namespace Tree
{
	struct EngineStartupConfig
	{
		EDomain domain;
	};

	class EngineSystem : public IEngineSystem
	{
	public:
		void SetStartupConfig( EngineStartupConfig config );
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		void Run();

	private:
		EDomain m_domain;
	};
}