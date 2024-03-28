#pragma once

#include "Core.h"

namespace Tree
{
	namespace Sap
	{
		struct Memory
		{
			static void* AllocHGlobal( size_t InSize );
			static void FreeHGlobal( void* InPtr );

			static SapChar* StringToCoTaskMemAuto( SapStringView InString );
			static void FreeCoTaskMem( void* InMemory );
		};
	}
}
