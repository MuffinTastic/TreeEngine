#pragma once

#include "Tree.NativeCommon/system.h"

#define MANAGEDHOSTSYSTEM_NAME "ManagedHostSystem_v01"

namespace Tree
{
	enum SAP_GEN EManagedHostEvent
	{
		EMHEVENT_STARTUP,
		EMHEVENT_SHUTDOWN,
		EMHEVENT_UPDATE,
		EMHEVENT_PRESCENE_RENDER,
		EMHEVENT_POSTSCENE_PREUI_RENDER
	};

	class IManagedHostSystem : public ISystem
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;

		virtual void TrunkStartup() = 0;
		virtual void TrunkShutdown() = 0;

		virtual void TrunkUpdateHook() = 0;
		virtual void TrunkPreSceneRenderHook() = 0;
		virtual void TrunkPostScenePreUIRenderHook() = 0;
		virtual void TrunkPostUIRenderHook() = 0;
	};
}