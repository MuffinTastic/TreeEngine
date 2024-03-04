#pragma once

#include "interfaces/itestsystem.h"

namespace Tree
{
	class TestSystem : public ITestSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		virtual void RootSpecific() override;
	};
}