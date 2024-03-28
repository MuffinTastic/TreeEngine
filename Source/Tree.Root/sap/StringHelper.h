#pragma once

#include "Core.h"

namespace Tree
{
	namespace Sap
	{
		class StringHelper
		{
		public:
#if defined(SAP_WIDE_CHARS)
			static std::wstring ConvertUtf8ToWide( std::string_view InString );
			static std::string ConvertWideToUtf8( std::wstring_view InString );
#else
			static std::string ConvertUtf8ToWide( std::string_view InString );
			static std::string ConvertWideToUtf8( std::string_view InString );
#endif
		};
	}
}
