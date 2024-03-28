#include "String.h"
#include "StringHelper.h"
#include "Memory.h"
#include "Verify.h"

namespace Tree
{
	namespace Sap
	{
		String String::New( const char* InString )
		{
			String result;
			result.Assign( InString );
			return result;
		}

		String String::New( std::string_view InString )
		{
			String result;
			result.Assign( InString );
			return result;
		}

		void String::Free( String& InString )
		{
			if ( InString.m_String == nullptr )
				return;

			Memory::FreeCoTaskMem( InString.m_String );
			InString.m_String = nullptr;
		}

		void String::Assign( std::string_view InString )
		{
			if ( m_String != nullptr )
				Memory::FreeCoTaskMem( m_String );

			m_String = Memory::StringToCoTaskMemAuto( StringHelper::ConvertUtf8ToWide( InString ) );
		}

		String::operator std::string() const
		{
			SapStringView string( m_String );

#if defined(SAP_WIDE_CHARS)
			return StringHelper::ConvertWideToUtf8( string );
#else
			return std::string( string );
#endif
		}

		bool String::operator==( const String& InOther ) const
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

		bool String::operator==( std::string_view InOther ) const
		{
#if defined(SAP_WIDE_CHARS)
			auto str = StringHelper::ConvertUtf8ToWide( InOther );
			return wcscmp( m_String, str.data() ) == 0;
#else
			return strcmp( m_String, InOther.data() ) == 0;
#endif
		}
	}
}
