#pragma once

#include <memory>
#include <source_location>

#include <fmt/format.h>

#include "Tree.NativeCommon/system.h"
#include "Tree.NativeCommon/debug.h"

#include "Tree.Root/sap/Array.h"
#include "Tree.Root/sap/String.h"

#define LOGSYSTEM_NAME "LogSystem_v01"

namespace Tree
{
	struct ConsoleLogEntry
	{
		std::string loggerName;
		int logLevel = 0;
		uint64_t timestamp = 0;
		std::string text;
	};

	struct SAP_GEN ConsoleLogEntrySap
	{
		Sap::String loggerName;
		int logLevel;
		uint64_t timestamp;
		Sap::String text;
	};

	class ILogger
	{
	public:
		template <typename... Args>
		void Info( const LogFormat format, const Args&... arguments )
		{
			std::string formatted = fmt::vformat( format.m_text, fmt::make_format_args( arguments... ) );
			InternalInfo( 
				format.file_name(),
				format.line(),
				format.function_name(),
				formatted
			);
		}

		template <typename... Args>
		void Warning( const LogFormat format, const Args&... arguments )
		{
			std::string formatted = fmt::vformat( format.m_text, fmt::make_format_args( arguments... ) );
			InternalWarning(
				format.file_name(),
				format.line(),
				format.function_name(),
				formatted
			);
		}

		template <typename... Args>
		void Error( const LogFormat format, const Args&... arguments )
		{
			std::string formatted = fmt::vformat( format.m_text, fmt::make_format_args( arguments... ) );
			InternalError(
				format.file_name(),
				format.line(),
				format.function_name(),
				formatted
			);
		}

		template <typename... Args>
		void Trace( const LogFormat format, const Args&... arguments )
		{
			std::string formatted = fmt::vformat( format.m_text, fmt::make_format_args( arguments... ) );
			InternalTrace(
				format.file_name(),
				format.line(),
				format.function_name(),
				formatted
			);
		}

	public:
		SAP_GEN virtual void InternalInfo( const char* file, int line, const char* function, const std::string_view text ) = 0;
		SAP_GEN virtual void InternalWarning( const char* file, int line, const char* function, const std::string_view text ) = 0;
		SAP_GEN virtual void InternalError( const char* file, int line, const char* function, const std::string_view text ) = 0;
		SAP_GEN virtual void InternalTrace( const char* file, int line, const char* function, const std::string_view text ) = 0;
	};

	class ILogSystem : public ISystem, virtual public ILogger
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;

		SAP_GEN virtual ILogger* CreateLogger( std::string name ) = 0;
		SAP_GEN virtual const Sap::Array<ConsoleLogEntrySap> GetConsoleLogHistorySap() const = 0;
	};
}