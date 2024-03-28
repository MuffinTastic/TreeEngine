#include "StringHelper.h"

#include "Tree.NativeCommon/unicode.h"

namespace Tree
{
	namespace Sap
	{
#if defined(SAP_WIDE_CHARS)
		std::wstring StringHelper::ConvertUtf8ToWide( std::string_view str )
		{
			return utf8::utf8towchar( str );
		}

		std::string StringHelper::ConvertWideToUtf8( std::wstring_view str )
		{
			return utf8::wcharto8( str );
		}

#else
		std::string StringHelper::ConvertUtf8ToWide( std::string_view str )
		{
			return std::string( str );
		}

		std::string StringHelper::ConvertWideToUtf8( std::string_view str )
		{
			return std::string( str );
		}
#endif

	}
}
