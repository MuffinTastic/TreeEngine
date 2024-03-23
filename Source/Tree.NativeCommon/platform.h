#pragma once

#include <string>
#include <functional>
#include <filesystem>

#include <fmt/format.h>

namespace Tree::Platform
{
	// These return absolute (fully qualified) paths
	std::filesystem::path GetExecutableFilePath();
	std::filesystem::path GetExecutableDirectoryPath();
	std::filesystem::path GetEngineDirectoryPath();
	std::filesystem::path GetLogDirectoryPath();

	std::string GetExecutableFileUTF8();
	std::string GetExecutableDirectoryUTF8();
	std::string GetEngineDirectoryUTF8();
	std::string GetLogDirectoryUTF8();


	std::string PathToUTF8( std::filesystem::path path );
	std::filesystem::path UTF8ToPath( std::string_view str );

	struct SharedLibrary;
	SharedLibrary* LoadSharedLibrary( std::string path );
	void UnloadSharedLibrary( SharedLibrary* sharedLibrary );

	void* GetSharedLibraryFunc( SharedLibrary* sharedLibrary, std::string name );

	template <typename T>
	std::function<T> GetSharedLibraryFunc( SharedLibrary* sharedLibrary, std::string name )
	{
		void* ptr = Platform::GetSharedLibraryFunc( sharedLibrary, name );
		std::function<T> func( reinterpret_cast<T*>( ptr ) );
		return func;
	}

	void ChangeCurrentDirectoryPath( std::filesystem::path path );
	void ChangeCurrentDirectoryUTF8( std::string path );

	void DebugLog( std::string text );

	template <typename... Args>
	void DebugLog( const std::string_view format, const Args&... arguments )
	{
		std::string formatted = fmt::vformat( format, fmt::make_format_args( arguments... ) );
		DebugLog( formatted );
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