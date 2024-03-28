#include "interfaces/ilogsystem.h"

#include <atomic>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <utility>
#include <charconv>

#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#define SPDLOG_WCHAR_FILENAMES
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include "spdlog/sinks/basic_file_sink.h"

#include "Tree.NativeCommon/sys.h"
#include "Tree.NativeCommon/unicode.h"
#include "Tree.NativeCommon/string.h"

namespace Tree
{
	static const size_t MaxConsoleLogEntries = 256;

	template<class Mutex>
	class ConsoleLogSink : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		explicit ConsoleLogSink() {}
		~ConsoleLogSink() {};

		const ConsoleLogHistorySap GetSap();

	protected:
		using spdlog::sinks::base_sink<Mutex>::formatter_;
		using spdlog::sinks::base_sink<Mutex>::mutex_;

		virtual void sink_it_( const spdlog::details::log_msg& msg ) override;
		virtual void flush_() override {};

	private:
		std::vector<ConsoleLogEntry> m_logEntries;
	};

	using ConsoleLogSinkMT = ConsoleLogSink<std::mutex>;

#if WINDOWS
	template<class Mutex>
	class UnicodeMSVCSink : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		explicit UnicodeMSVCSink() {}

	protected:
		using spdlog::sinks::base_sink<Mutex>::formatter_;

		virtual void sink_it_( const spdlog::details::log_msg& msg ) override;
		virtual void flush_() override {}
	};

	using UnicodeMSVCSinkMT = UnicodeMSVCSink<std::mutex>;
#endif

	class Logger : virtual public ILogger
	{
	public:
		Logger() {}
		Logger( std::string name, std::vector<spdlog::sink_ptr> sinks );

		void SetLogger( std::string name, std::vector<spdlog::sink_ptr> sinks );

		virtual void InternalInfo( const char* file, int line, const char* function, const std::string_view text ) override;
		virtual void InternalWarning( const char* file, int line, const char* function, const std::string_view text ) override;
		virtual void InternalError( const char* file, int line, const char* function, const std::string_view text ) override;
		virtual void InternalTrace( const char* file, int line, const char* function, const std::string_view text ) override;
	
	protected:
		std::shared_ptr<spdlog::logger> m_spdlogger;
	};

	// We love inheritance diamonds
	// Not to fret, ILogSystem doesn't implement any ILogger methods
#pragma warning( push )
#pragma warning( disable : 4250 )
	class LogSystem : public ILogSystem, public Logger
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		// This doesn't actually do anything, it still gives the annoying warning about inheriting via dominance.
		// I added the warning pragma to make it go away, but I'll still leave this here
		// to be explicit about what's going on.
		using Logger::InternalInfo;
		using Logger::InternalWarning;
		using Logger::InternalError;
		using Logger::InternalTrace;

		virtual std::shared_ptr<ILogger> CreateLogger( std::string name ) override;

		virtual ILogger* CreateLoggerSap( std::string name ) override;
		virtual const ConsoleLogHistorySap GetConsoleLogHistorySap() const override;

	private:
		static constexpr const char* s_logFileName = "log.latest.txt";
		static constexpr const char* s_oldLogRenameFormat = "log.{}.txt";
		static constexpr const char* s_oldLogRegexFormat = "^log\\.(\\d+)\\.txt$";
		static constexpr int s_oldLogCountDefault = 10;

		spdlog::sink_ptr m_stdoutLogSink;
#if WINDOWS
		spdlog::sink_ptr m_msvcLogSink;
#endif
		std::shared_ptr<ConsoleLogSinkMT> m_consoleLogSink;
		spdlog::sink_ptr m_fileLogSink;
		std::string m_logFilePathStr;

		std::shared_ptr<ILogger> m_systemLogger;

		void ManageOldLogFiles();
		void CreateSinks();
		void SetPatterns();
		std::vector<spdlog::sink_ptr> GetSinks() const;
	};
#pragma warning( pop )
}

#pragma region Sink Implementations

template<class Mutex>
const Tree::ConsoleLogHistorySap Tree::ConsoleLogSink<Mutex>::GetSap()
{
	std::lock_guard<Mutex> lock( mutex_ );

	size_t count = m_logEntries.size();

	ConsoleLogHistorySap sap;
	sap.count = static_cast<int>( count );
	sap.entries = new ConsoleLogEntrySap[count];

	for ( int i = 0; i < count; ++i )
	{
		auto& sapEntry = sap.entries[i];
		auto& logEntry = m_logEntries[i];

		sapEntry.loggerName = CopyToCString( logEntry.loggerName );
		sapEntry.logLevel = logEntry.logLevel;
		sapEntry.text = CopyToCString( logEntry.text );
	}

	return sap;
}

template<class Mutex>
void Tree::ConsoleLogSink<Mutex>::sink_it_( const spdlog::details::log_msg& msg )
{
	spdlog::memory_buf_t formatted;
	formatter_->format( msg, formatted );
	
	ConsoleLogEntry entry;
	entry.loggerName = fmt::format( "{}", msg.logger_name );
	entry.logLevel = static_cast<int>( msg.level );
	entry.text = std::string( formatted.begin(), formatted.end() );

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
	return new Logger( name, GetSinks() );;
}

const Tree::ConsoleLogHistorySap Tree::LogSystem::GetConsoleLogHistorySap() const
{
	return m_consoleLogSink->GetSap();
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

	m_consoleLogSink->set_pattern( "[%H:%M:%S] [%n] [%l] %v" );
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