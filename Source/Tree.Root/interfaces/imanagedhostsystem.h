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

		virtual void TrunkRunEvent( EManagedHostEvent event ) = 0;
	};
}