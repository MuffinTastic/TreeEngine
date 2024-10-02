#pragma once

#include "Tree.NativeCommon/system.h"

#define MEMORYSYSTEM_NAME "MemorySystem_v01"

namespace Tree
{
	// DANGER - changing this will break compat with other modules
	enum EMemoryCategory : int
	{
		EMEMCATEGORY_GENERAL,
		EMEMCATEGORY_RENDER,
		EMEMCATEGORY_AUDIO,

		EMEMCATEGORY_AMOUNT,
	};

	class IMemorySystem : public ISystem
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;

		virtual void* Allocate( EMemoryCategory category, size_t size ) = 0;
		virtual void Free( EMemoryCategory category, void* p ) = 0;
	};
}