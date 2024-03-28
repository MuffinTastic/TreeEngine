#pragma once

#include <string>

#ifdef __clang__
#define TREE_HAS_SOURCE_LOCATION 0
#else
#define TREE_HAS_SOURCE_LOCATION __has_include(<source_location>)
#endif

#if TREE_HAS_SOURCE_LOCATION
#include <source_location>

#define SAP_SOURCE_LOCATION std::source_location location = std::source_location::current(); const char* file = location.file_name(); int line = location.line()
#else
#define SAP_SOURCE_LOCATION const char* file = __FILE__; int line = __LINE__
#endif

#if defined(__clang__)
#define TREE_DEBUG_BREAK __builtin_debugtrap()
#elif defined(_MSC_VER)
#define TREE_DEBUG_BREAK __debugbreak()
#else
#define TREE_DEBUG_BREAK	
#endif

namespace Tree
{
	struct LogFormat
	{
		std::string m_text;
		std::source_location m_location;

		LogFormat( const char* text, const std::source_location location = std::source_location::current() )
			: m_text( text ), m_location( location )
		{

		}

		LogFormat( const std::string_view text, const std::source_location location = std::source_location::current() )
			: m_text( text ), m_location( location )
		{

		}

		LogFormat( const std::string text, const std::source_location location = std::source_location::current() )
			: m_text( text ), m_location( location )
		{

		}
	};
}