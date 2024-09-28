#pragma once

#include <string>
#include <functional>
#include <filesystem>

#include <fmt/format.h>

#include "defs.h"
#include "debug.h"

namespace Tree::Platform
{
	// These return absolute (fully qualified) paths
	std::filesystem::path GetExecutableFilePath();
	std::filesystem::path GetExecutableDirectoryPath();
	std::filesystem::path GetEngineDirectoryPath();
	std::filesystem::path GetLogDirectoryPath();

	SAP_GEN std::string GetExecutableFileUTF8();
	SAP_GEN std::string GetExecutableDirectoryUTF8();
	SAP_GEN std::string GetEngineDirectoryUTF8();
	SAP_GEN std::string GetLogDirectoryUTF8();


	std::string PathToUTF8( std::filesystem::path path );
	std::filesystem::path UTF8ToPath( std::string_view str );

	struct PlatformModule;
	PlatformModule* LoadModule( std::filesystem::path path );
	void UnloadModule( PlatformModule* module );

	void* GetModuleFunc( PlatformModule* module, std::string name );

	template <typename T>
	std::function<T> GetModuleFunc( PlatformModule* module, std::string name )
	{
		void* ptr = Platform::GetModuleFunc( module, name );
		std::function<T> func( reinterpret_cast<T*>( ptr ) );
		return func;
	}

	template <typename T>
	T GetModuleFuncPtr( PlatformModule* module, std::string name )
	{
		void* ptr = Platform::GetModuleFunc( module, name );
		return reinterpret_cast<T>( ptr );
	}

	void ChangeCurrentDirectoryPath( std::filesystem::path path );
	void ChangeCurrentDirectoryUTF8( std::string path );

	void InternalDebugLog( const char* file, int line, const char* function, const std::string_view text );

	template <typename... Args>
	void DebugLog( const LogFormat format, const Args&... arguments )
	{
		std::string formatted = fmt::vformat( format.m_text, fmt::make_format_args( arguments... ) );
		InternalDebugLog(
			format.file_name(),
			format.line(),
			format.function_name(),
			formatted
		);
	}

	void FatalExit();
}

#define EXPORT extern "C" __declspec( dllexport )

#ifdef WINDOWS
#define SHAREDLIB_EXT ".dll"
#elif LINUX
#define SHAREDLIB_EXT ".so"
#endif

#ifndef DEDICATED_SERVER
#define GUI_ENABLED
#endif