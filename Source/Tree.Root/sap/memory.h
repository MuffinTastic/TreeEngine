#pragma once

#include "Core.h"

#ifdef WINDOWS
#include <combaseapi.h>
#include <string.h>
#endif

namespace Tree
{
	namespace Sap
	{
		struct Memory
		{
			inline static void* AllocHGlobal( size_t InSize )
			{
#ifdef WINDOWS
				return LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, InSize );
#else
				return malloc( InSize );
#endif
			}
			inline static void FreeHGlobal( void* InPtr )
			{
#ifdef WINDOWS
				LocalFree( InPtr );
#else
				free( InPtr );
#endif
			}

			inline static SapChar* StringToCoTaskMemAuto( SapStringView InString )
			{
				size_t length = InString.length() + 1;
				size_t size = length * sizeof( SapChar );

#ifdef WINDOWS
				auto* buffer = static_cast<SapChar*>( CoTaskMemAlloc( size ) );

				if ( buffer != nullptr )
				{
					memset( buffer, 0xCE, size );
					wcscpy_s( buffer, length, InString.data() );
				}
#else
				auto* buffer = static_cast<SapChar*>( AllocHGlobal( size ) );

				if ( buffer != nullptr )
				{
					memset( buffer, 0, size );
					strcpy_s( buffer, length, InString.data() );
				}
#endif

				return buffer;
			}

			inline static void FreeCoTaskMem( void* InMemory )
			{
#ifdef WINDOWS
				CoTaskMemFree( InMemory );
#else
				FreeHGlobal( InMemory );
#endif
			}
		};
	}
}
