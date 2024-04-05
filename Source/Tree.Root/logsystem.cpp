#include "logsystem.h"

#include <atomic>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <utility>
#include <charconv>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include "spdlog/sinks/basic_file_sink.h"

#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "Tree.NativeCommon/sys.h"
#include "Tree.NativeCommon/unicode.h"
#include "Tree.NativeCommon/string.h"

#pragma region Sink Implementations

template<class Mutex>
const std::vector<Tree::ConsoleLogEntry> Tree::ConsoleLogSink<Mutex>::GetHistory()
{
	std::lock_guard<Mutex> lock( mutex_ );
	return m_logEntries;
}

template<class Mutex>
void Tree::ConsoleLogSink<Mutex>::sink_it_( const spdlog::details::log_msg& msg )
{
	ConsoleLogEntry entry;
	entry.loggerName = std::string( msg.logger_name.begin(), msg.logger_name.end() );
	entry.logLevel = static_cast<int>( msg.level );
	entry.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>( msg.time.time_since_epoch() ).count();
	entry.text = std::string( msg.payload.begin(), msg.payload.end() );

	m_logEntries.push_back( entry );

	if ( m_logEntries.size() > MaxConsoleLogEntries )
	{
		m_logEntries.erase( m_logEntries.begin() );
	}
}


#if WINDOWS
template<class Mutex>
void Tree::UnicodeMSVCSink<Mutex>::sink_it_( const spdlog::details::log_msg& msg )
{
	spdlog::memory_buf_t formatted;
	formatter_->format( msg, formatted );

	std::string_view text( formatted.begin(), formatted.end() );

	std::wstring wctext = utf8::utf8towchar( text );
	OutputDebugStringW( wctext.data() );
}
#endif

#pragma endregion

#pragma region Logger Implementations

Tree::Logger::Logger( std::string name, std::vector<spdlog::sink_ptr> sinks )
{
	SetLogger( name, sinks );
}

void Tree::Logger::SetLogger( std::string name, std::vector<spdlog::sink_ptr> sinks )
{
	m_spdlogger = spdlog::get( name );

	if ( !m_spdlogger )
	{
		m_spdlogger = std::make_shared<spdlog::logger>( name, sinks.begin(), sinks.end() );
		spdlog::register_logger( m_spdlogger );
	}
}

void Tree::Logger::InternalInfo( const char* file, int line, const char* function, const std::string_view text )
{
	m_spdlogger->log( spdlog::source_loc{ file, line, function }, spdlog::level::level_enum::info, text );
}

void Tree::Logger::InternalWarning( const char* file, int line, const char* function, const std::string_view text )
{
	m_spdlogger->log( spdlog::source_loc{ file, line, function }, spdlog::level::level_enum::warn, text );
}

void Tree::Logger::InternalError( const char* file, int line, const char* function, const std::string_view text )
{
	m_spdlogger->log( spdlog::source_loc{ file, line, function }, spdlog::level::level_enum::err, text );
}

void Tree::Logger::InternalTrace( const char* file, int line, const char* function, const std::string_view text )
{
	m_spdlogger->log( spdlog::source_loc{ file, line, function }, spdlog::level::level_enum::trace, text );
}

#pragma endregion

#pragma region LogSystem Implementations

REGISTER_TREE_SYSTEM( LogSystem, LOGSYSTEM_NAME )


Tree::ESystemInitCode Tree::LogSystem::Startup()
{
	CreateSinks();

	SetLogger( "Default", GetSinks() );
	spdlog::set_default_logger( m_spdlogger );

	SetPatterns();

	m_systemLogger = CreateLogger( "LogSystem" );
	m_systemLogger->Info( "Started logging" );
	if ( m_fileLogSink )
		m_systemLogger->Info( "Opened log file at {}", m_logFilePathStr );

    return ESYSTEMINIT_SUCCESS;
}

void Tree::LogSystem::Shutdown()
{

}

std::shared_ptr<Tree::ILogger> Tree::LogSystem::CreateLogger( std::string name )
{
	return std::make_shared<Logger>( name, GetSinks() );
}

Tree::ILogger* Tree::LogSystem::CreateLoggerSap( std::string name )
{
	auto smartPtr = CreateLogger( name );
	m_managedLoggers.push_back( smartPtr );
	return smartPtr.get();
}

const Tree::Sap::Array<Tree::ConsoleLogEntrySap> Tree::LogSystem::GetConsoleLogHistorySap() const
{
	auto entries = m_consoleLogSink->GetHistory();

	auto array = Sap::Array<ConsoleLogEntrySap>::New( entries.size() );
	for ( size_t i = 0; i < entries.size(); ++i )
	{
		ConsoleLogEntrySap sapEntry
		{
			.loggerName = Sap::String::New( entries[i].loggerName ),
			.logLevel = entries[i].logLevel,
			.timestamp = entries[i].timestamp,
			.text = Sap::String::New( entries[i].text )
		};

		array[i] = sapEntry;
	}

	return array;
}

void Tree::LogSystem::ManageOldLogFiles()
{
	std::filesystem::path logPath = Platform::GetLogDirectoryPath();
	if ( !std::filesystem::exists( logPath ) )
	{
		return;
	}

	//
	// Take stock of old logs
	//

	std::vector<std::pair<int, std::filesystem::path>> oldLogs;

	{
		std::regex oldLogRegex( s_oldLogRegexFormat );

		for ( const auto& entry : std::filesystem::directory_iterator( Platform::GetLogDirectoryPath() ) )
		{
			auto path = entry.path();
			std::string filename = path.filename().string();
			std::smatch matches;
		
			if ( !std::regex_search( filename, matches, oldLogRegex) )
			{
				continue;
			}

			std::string oldLogNumStr = matches[1].str();
			int oldLogNum;

			auto result = std::from_chars( oldLogNumStr.data(), oldLogNumStr.data() + oldLogNumStr.size(), oldLogNum );

			if ( result.ec != std::errc{} )
			{
				// We had an error converting to int, probably invalid, skip it
				continue;
			}

			oldLogs.push_back( std::make_pair( oldLogNum, path ) );
		}

		std::sort( oldLogs.begin(), oldLogs.end(),
			[]( auto& first, auto& second ) { return first.first < second.first; } );
	}

	//
	// Rename last log
	//

	{
		auto lastLogFilePath = logPath / s_logFileName;

		if ( std::filesystem::exists( lastLogFilePath ) )
		{
			int oldLogNum = 1;

			if ( !oldLogs.empty() )
			{
				oldLogNum = oldLogs.back().first + 1;
			}

			std::string newName = fmt::format( s_oldLogRenameFormat, oldLogNum );
			auto newPath = logPath / newName;

			std::filesystem::rename( lastLogFilePath, newPath );
			oldLogs.push_back( std::make_pair( oldLogNum, newPath ) );
		}
	}

	//
	// Delete logs that are too old
	//

	if ( !Sys::CmdLine()->GetFlag( "keep-old-logs" ) )
	{
		int keepLogCount = Sys::CmdLine()->GetIntOption( "keep-old-logs", s_oldLogCountDefault );
		int overCap = static_cast<int>( oldLogs.size() ) - keepLogCount;

		for ( int i = 0; i < overCap; ++i )
		{
			std::filesystem::remove( oldLogs[i].second );
		}
	}
}

void Tree::LogSystem::CreateSinks()
{
	if ( Sys::CmdLine()->GetFlag( "log-no-color" ) )
	{
		m_stdoutLogSink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
	}
	else
	{
		m_stdoutLogSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	}

#if WINDOWS
	m_msvcLogSink = std::make_shared<UnicodeMSVCSinkMT>();
#endif

	m_consoleLogSink = std::make_shared<ConsoleLogSinkMT>();

	try
	{
		std::filesystem::path logFilePath;

		if ( !Sys::CmdLine()->GetFlag( "log-file" ) )
		{
			ManageOldLogFiles();
			std::filesystem::path logPath = Platform::GetLogDirectoryPath();
			logFilePath = logPath / Platform::UTF8ToPath( s_logFileName );

			if ( !std::filesystem::exists( logPath ) )
			{
				std::filesystem::create_directories( logPath );
			}
		}
		else
		{
			logFilePath = Platform::UTF8ToPath( Sys::CmdLine()->GetStringOption( "log-file", s_logFileName ) );
		}

#if WINDOWS
		// I hate C++
		m_fileLogSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>( logFilePath.wstring() );
#else
		m_fileLogSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>( logFilePath.string() );
#endif

		m_logFilePathStr = Platform::PathToUTF8( logFilePath );
	}
	catch ( const spdlog::spdlog_ex& ex )
	{
		Platform::DebugLog( "Log init failed: {}", ex.what() );
	}
}

void Tree::LogSystem::SetPatterns()
{
	const char* nocolor_pattern = "[%H:%M:%S] [%n]\t[%l] [%s:%#:%!] %v";
	const char* color_pattern = "[%H:%M:%S] [%n]\t[%^%l%$] [%s:%#:%!] %v";

	const char* pattern;
	if ( Sys::CmdLine()->GetFlag( "nocolor" ) )
		pattern = nocolor_pattern;
	else
		pattern = color_pattern;

	spdlog::set_pattern( pattern );
}

std::vector<spdlog::sink_ptr> Tree::LogSystem::GetSinks() const
{
	std::vector<spdlog::sink_ptr> sinks =
	{
		m_stdoutLogSink,
		m_consoleLogSink,
#if WINDOWS
		m_msvcLogSink
#endif
	};

	if ( m_fileLogSink )
		sinks.push_back( m_fileLogSink );

	return sinks;
}

#pragma endregion