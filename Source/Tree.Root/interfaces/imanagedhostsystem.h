#pragma once

#include "Tree.NativeCommon/system.h"
#include "Tree.Root/sap/Core.h"

#define MANAGEDHOSTSYSTEM_NAME "ManagedHostSystem_v01"

namespace Tree
{
	class IManagedHostSystem : public ISystem
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;

		virtual Sap::Bool32 TrunkStartup() = 0;
		virtual void TrunkShutdown() = 0;

		virtual void TrunkUpdateHook() = 0;
		virtual void TrunkPreSceneRenderHook() = 0;
		virtual void TrunkPostScenePreUIRenderHook() = 0;
		virtual void TrunkPostUIRenderHook() = 0;
	};
}