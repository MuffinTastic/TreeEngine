#include "Memory.h"

#ifdef WINDOWS
#include <combaseapi.h>
#endif

namespace Tree
{
	namespace Sap
	{
		void* Memory::AllocHGlobal( size_t InSize )
		{
#ifdef WINDOWS
			return LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, InSize );
#else
			return malloc( InSize );
#endif
		}

		void Memory::FreeHGlobal( void* InPtr )
		{
#ifdef WINDOWS
			LocalFree( InPtr );
#else
			free( InPtr );
#endif
		}

		SapChar* Memory::StringToCoTaskMemAuto( SapStringView InString )
		{
			size_t length = InString.length() + 1;
			size_t size = length * sizeof( SapChar );

#ifdef WINDOWS
			auto* buffer = static_cast<SapChar*>( CoTaskMemAlloc( size ) );

			if ( buffer != nullptr )
			{
				memset( buffer, 0xCE, size );
				wcscpy( buffer, InString.data() );
			}
#else
			auto* buffer = static_cast<SapChar*>( AllocHGlobal( size ) );

			if ( buffer != nullptr )
			{
				memset( buffer, 0, size );
				strcpy( buffer, InString.data() );
			}
#endif

			return buffer;
		}

		void Memory::FreeCoTaskMem( void* InMemory )
		{
#ifdef WINDOWS
			CoTaskMemFree( InMemory );
#else
			FreeHGlobal( InMemory );
#endif
		}
	}
}
