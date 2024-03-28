// Modified from https://github.com/StudioCherno/Coral

// MIT License
// 
// Copyright( c ) 2023 Studio Cherno
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "common.h"

#ifdef WINDOWS
#include <combaseapi.h>
#endif

namespace Tree
{
	namespace Sap
	{
		inline void* AllocHGlobal( size_t size )
		{
#ifdef WINDOWS
			return LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, size );
#else
			return malloc( size );
#endif
		}

		inline void FreeHGlobal( void* ptr )
		{
#ifdef WINDOWS
			LocalFree( ptr );
#else
			free( ptr );
#endif
		}

		inline SapChar* StringToCoTaskMemAuto( SapStringView str )
		{
			size_t length = str.length() + 1;
			size_t size = length * sizeof( SapChar );

#ifdef WINDOWS
			auto* buffer = static_cast<SapChar*>( CoTaskMemAlloc( size ) );

			if ( buffer != nullptr )
			{
				memset( buffer, 0xCE, size );
				wcscpy( buffer, str.data() );
			}
#else
			auto* buffer = static_cast<SapChar*>( AllocHGlobal( size ) );

			if ( buffer != nullptr )
			{
				memset( buffer, 0, size );
				strcpy( buffer, str.data() );
			}
#endif

			return buffer;
		}

		inline void FreeCoTaskMem( void* memory )
		{
#ifdef WINDOWS
			CoTaskMemFree( memory );
#else
			FreeHGlobal( memory );
#endif
		}
	}
}