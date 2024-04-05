#pragma once

#include <vector>
#include <memory>

#define SPDLOG_WCHAR_FILENAMES
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>

#include "interfaces/ilogsystem.h"

namespace Tree
{
	static const size_t MaxConsoleLogEntries = 256;

	template<class Mutex>
	class ConsoleLogSink : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		explicit ConsoleLogSink() {}
		~ConsoleLogSink() {};

		const std::vector<ConsoleLogEntry> GetHistory();

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
		virtual const Sap::Array<ConsoleLogEntrySap> GetConsoleLogHistorySap() const override;

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
		std::vector<std::shared_ptr<ILogger>> m_managedLoggers;

		void ManageOldLogFiles();
		void CreateSinks();
		void SetPatterns();
		std::vector<spdlog::sink_ptr> GetSinks() const;
	};
#pragma warning( pop )
}