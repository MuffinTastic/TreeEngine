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
#include "memory.h"

#include "Tree.NativeCommon/unicode.h"

namespace Tree
{
	namespace Sap
	{
		namespace StringHelper
		{
	#ifdef SAP_WIDE_CHARS
			inline static std::wstring ConvertUtf8ToWide( std::string_view str )
			{
				return utf8::utf8towchar( str );
			}

			inline static std::string ConvertWideToUtf8( std::wstring_view str )
			{
				return utf8::wcharto8( str );
			}
	#else
			inline static std::string ConvertUtf8ToWide( std::string_view str )
			{
				return std::string( str );
			}

			inline static std::string ConvertWideToUtf8( std::string_view str )
			{
				return std::string( str );
			}
	#endif
		}

		class SapString
		{
		public:
			inline static SapString New( const char* str )
			{
				SapString result;
				result.Assign( str );
				return result;
			}


			inline static SapString New( std::string_view str )
			{
				SapString result;
				result.Assign( str );
				return result;
			}


			inline static void Free( SapString& str )
			{
				if ( str.m_str == nullptr )
					return;

				FreeCoTaskMem( str.m_str );
				str.m_str = nullptr;
			}

			void Assign( std::string_view str )
			{
				if ( m_str != nullptr )
					FreeCoTaskMem( m_str );

				m_str = StringToCoTaskMemAuto( StringHelper::ConvertUtf8ToWide( str ) );
			}

			inline operator std::string() const
			{
				SapStringView string( m_str );

#ifdef SAP_WIDE_CHARS
				return StringHelper::ConvertWideToUtf8( string );
#else
				return std::string( string );
#endif
			}

			inline bool operator==( const SapString& other ) const
			{
				if ( m_str == other.m_str )
					return true;

				if ( m_str == nullptr || other.m_str == nullptr )
					return false;

#ifdef SAP_WIDE_CHARS
				return wcscmp( m_str, other.m_str ) == 0;
#else
				return strcmp( m_str, other.m_str ) == 0;
#endif
			}

			inline bool operator==( std::string_view other ) const
			{
#ifdef SAP_WIDE_CHARS
				auto str = StringHelper::ConvertUtf8ToWide( other );
				return wcscmp( m_str, str.data() ) == 0;
#else
				return strcmp( m_str, str.data() ) == 0;
#endif
			}

			inline SapChar* Data() { return m_str; }
			inline const SapChar* Data() const { return m_str; }

		private:
			SapChar* m_str = nullptr;
			SapBool32 m_IsDisposed = false;
		};

		struct ScopedSapString
		{
			ScopedSapString( SapString str )
				: m_str( str )
			{
			}

			ScopedSapString& operator=( SapString other )
			{
				SapString::Free( m_str );
				m_str = other;
				return *this;
			}

			ScopedSapString& operator=( const ScopedSapString& other )
			{
				SapString::Free( m_str );
				m_str = other.m_str;
				return *this;
			}

			~ScopedSapString()
			{
				SapString::Free( m_str );
			}

			inline operator std::string() const { return m_str; }
			inline operator SapString() const { return m_str; }

			inline bool operator==( const ScopedSapString& other ) const
			{
				return m_str == other.m_str;
			}

			inline bool operator==( std::string_view other ) const
			{
				return m_str == other;
			}

		private:
			SapString m_str;
		};
	}
}