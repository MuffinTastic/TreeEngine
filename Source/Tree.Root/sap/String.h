#pragma once

#include "Core.h"
#include "StringHelper.h"
#include "Memory.h"
#include "Verify.h"

namespace Tree
{
	namespace Sap
	{
		class String
		{
		public:
			inline static String New( const char* InString )
			{
				String result;
				result.Assign( InString );
				return result;
			}

			inline static String New( std::string_view InString )
			{
				String result;
				result.Assign( InString );
				return result;
			}

			inline static void Free( String& InString )
			{
				if ( InString.m_String == nullptr )
					return;

				Memory::FreeCoTaskMem( InString.m_String );
				InString.m_String = nullptr;
			}

			inline void Assign( std::string_view InString )
			{
				if ( m_String != nullptr )
					Memory::FreeCoTaskMem( m_String );

				m_String = Memory::StringToCoTaskMemAuto( StringHelper::ConvertUtf8ToWide( InString ) );
			}

			inline operator std::string() const
			{
				SapStringView string( m_String );

#if defined(SAP_WIDE_CHARS)
				return StringHelper::ConvertWideToUtf8( string );
#else
				return std::string( string );
#endif
			}

			inline bool operator==( const String& InOther ) const
			{
				if ( m_String == InOther.m_String )
					return true;

				if ( m_String == nullptr || InOther.m_String == nullptr )
					return false;

#if defined(SAP_WIDE_CHARS)
				return wcscmp( m_String, InOther.m_String ) == 0;
#else
				return strcmp( m_String, InOther.m_String ) == 0;
#endif
			}

			inline bool operator==( std::string_view InOther ) const
			{
#if defined(SAP_WIDE_CHARS)
				auto str = StringHelper::ConvertUtf8ToWide( InOther );
				return wcscmp( m_String, str.data() ) == 0;
#else
				return strcmp( m_String, InOther.data() ) == 0;
#endif
			}

			SapChar* Data() { return m_String; }
			const SapChar* Data() const { return m_String; }

		private:
			SapChar* m_String = nullptr;
			Bool32 m_IsDisposed = false;
		};

		struct ScopedString
		{
			ScopedString( String InString )
				: m_String( InString )
			{
			}

			ScopedString& operator=( String InOther )
			{
				String::Free( m_String );
				m_String = InOther;
				return *this;
			}

			ScopedString& operator=( const ScopedString& InOther )
			{
				String::Free( m_String );
				m_String = InOther.m_String;
				return *this;
			}

			~ScopedString()
			{
				String::Free( m_String );
			}

			operator std::string() const { return m_String; }
			operator String() const { return m_String; }

			bool operator==( const ScopedString& InOther ) const
			{
				return m_String == InOther.m_String;
			}

			bool operator==( std::string_view InOther ) const
			{
				return m_String == InOther;
			}

		private:
			String m_String;
		};
	}
}
