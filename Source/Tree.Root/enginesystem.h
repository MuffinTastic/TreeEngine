#pragma once

#include "interfaces/ienginesystem.h"

namespace Tree
{
	class EngineSystem : public IEngineSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;
	};
}