#pragma once

#include "Core.h"

#include "Tree.NativeCommon/unicode.h"

namespace Tree
{
	namespace Sap
	{
		class StringHelper
		{
		public:
#if defined(SAP_WIDE_CHARS)
			inline static std::wstring ConvertUtf8ToWide( std::string_view InString )
			{
				return utf8::utf8towchar( InString );
			}

			inline static std::string ConvertWideToUtf8( std::wstring_view InString )
			{
				return utf8::wcharto8( InString );
			}
#else
			static std::string ConvertUtf8ToWide( std::string_view InString )
			{
				return std::string( InString );
			}

			static std::string ConvertWideToUtf8( std::string_view InString )
			{
				return std::string( InString );
			}
#endif
		};
	}
}
