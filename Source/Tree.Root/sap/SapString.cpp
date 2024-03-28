#include "SapString.h"
#include "StringHelper.h"
#include "Memory.h"
#include "Verify.h"

namespace Tree
{
	namespace Sap
	{
		SapString SapString::New( const char* InString )
		{
			SapString result;
			result.Assign( InString );
			return result;
		}

		SapString SapString::New( std::string_view InString )
		{
			SapString result;
			result.Assign( InString );
			return result;
		}

		void SapString::Free( SapString& InString )
		{
			if ( InString.m_String == nullptr )
				return;

			Memory::FreeCoTaskMem( InString.m_String );
			InString.m_String = nullptr;
		}

		void SapString::Assign( std::string_view InString )
		{
			if ( m_String != nullptr )
				Memory::FreeCoTaskMem( m_String );

			m_String = Memory::StringToCoTaskMemAuto( StringHelper::ConvertUtf8ToWide( InString ) );
		}

		SapString::operator std::string() const
		{
			SapStringView string( m_String );

#if defined(SAP_WIDE_CHARS)
			return StringHelper::ConvertWideToUtf8( string );
#else
			return std::string( string );
#endif
		}

		bool SapString::operator==( const SapString& InOther ) const
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

		bool SapString::operator==( std::string_view InOther ) const
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
