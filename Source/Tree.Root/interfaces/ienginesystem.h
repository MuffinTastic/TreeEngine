#pragma once

#include "Tree.NativeCommon/system.h"

#define ENGINESYSTEM_NAME "EngineSystem_v01"

namespace Tree
{
	class IEngineSystem : public System
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;
	};
}