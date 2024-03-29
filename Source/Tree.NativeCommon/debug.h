#pragma once

#include <cstdint>
#include <string>

#ifdef __clang__
	#if __has_include(<experimental/source_location>)
		#include <experimental/source_location>
		#define TREE_HAS_SOURCE_LOCATION
		namespace std
		{
			using source_location = std::experimental::source_location
		}
	#endif
#else
	#if __has_include(<source_location>)
		#include <source_location>
		#define TREE_HAS_SOURCE_LOCATION
	#endif
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

#ifdef TREE_HAS_SOURCE_LOCATION
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

		inline constexpr const char* file_name() const noexcept
		{
			return m_location.file_name();
		}

		inline constexpr uint_least32_t line() const noexcept
		{
			return m_location.line();
		}

		inline constexpr const char* function_name() const noexcept
		{
			return m_location.function_name();
		}
#else
		LogFormat( const char* text )
			: m_text( text )
		{

		}

		LogFormat( const std::string_view text )
			: m_text( text )
		{

		}

		LogFormat( const std::string text )
			: m_text( text )
		{

		}

		inline constexpr const char* file_name() const noexcept
		{
			return "";
		}

		inline constexpr uint_least32_t line() const noexcept
		{
			return 0;
		}

		inline constexpr const char* function_name() const noexcept
		{
			return "";
		}
#endif
	};
}