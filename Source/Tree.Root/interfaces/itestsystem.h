#pragma once

#include "Tree.NativeCommon/system.h"

#define TESTSYSTEM_NAME "TestSystem_v01"

namespace Tree
{
	class ITestSystem : public ISystem
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;
	};
}