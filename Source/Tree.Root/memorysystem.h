#pragma once

#include "interfaces/imemorysystem.h"

#include <cstdint>

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#endif

namespace Tree
{
	class MemorySystem : public IMemorySystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		virtual void* Allocate( EMemoryCategory category, size_t size ) override;
		virtual void Free( EMemoryCategory category, void* p ) override;

	private:
		size_t m_allocTrackers[EMEMCATEGORY_AMOUNT];

#ifdef WINDOWS
		HANDLE m_allocHeaps[EMEMCATEGORY_AMOUNT] = { NULL };
#endif
	};
}