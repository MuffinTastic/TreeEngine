#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include "unicode.h"

#include "Tree.NativeCommon/constants.h"


std::filesystem::path Tree::Platform::GetExecutableFilePath()
{
	wchar_t path[MAX_PATH];
	GetModuleFileNameW( NULL, path, MAX_PATH );
	std::u16string_view u16path( reinterpret_cast<char16_t*>( path ) );
	return std::filesystem::path( u16path );
}

std::filesystem::path Tree::Platform::GetExecutableDirectoryPath()
{
	return GetExecutableFilePath().parent_path();
}

std::filesystem::path Tree::Platform::GetEngineDirectoryPath()
{
	return GetExecutableDirectoryPath() / "Engine";
}

std::filesystem::path Tree::Platform::GetLogDirectoryPath()
{
	return GetExecutableDirectoryPath() / "Log";
}

std::string Tree::Platform::GetExecutableFileUTF8()
{
	return PathToUTF8( GetExecutableFilePath() );
}

std::string Tree::Platform::GetExecutableDirectoryUTF8()
{
	return PathToUTF8( GetExecutableDirectoryPath() );
}

std::string Tree::Platform::GetEngineDirectoryUTF8()
{
	return PathToUTF8( GetEngineDirectoryPath() );
}

std::string Tree::Platform::GetLogDirectoryUTF8()
{
	return PathToUTF8( GetLogDirectoryPath() );
}

std::string Tree::Platform::PathToUTF8( std::filesystem::path path )
{
	return utf8::utf16to8( path.u16string() );
}

std::filesystem::path Tree::Platform::UTF8ToPath( std::string_view str )
{
	return std::filesystem::path( utf8::utf8to16( str ) );
}

Tree::Platform::SharedLibrary* Tree::Platform::LoadSharedLibrary( std::filesystem::path path )
{
	std::wstring wpath = path.wstring();
	HMODULE library = LoadLibraryExW( wpath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	return reinterpret_cast<SharedLibrary*>( library );
}

void Tree::Platform::UnloadSharedLibrary( SharedLibrary* sharedLibrary )
{
	HMODULE library = reinterpret_cast<HMODULE>( sharedLibrary );
	FreeLibrary( library );
}

void* Tree::Platform::GetSharedLibraryFunc( SharedLibrary* sharedLibrary, std::string name )
{
	HMODULE hModule = reinterpret_cast<HMODULE>( sharedLibrary );
	const char* cname = name.c_str();
	FARPROC address = GetProcAddress( hModule, cname );
	return reinterpret_cast<void*>( address );
}

void Tree::Platform::ChangeCurrentDirectoryPath( std::filesystem::path path )
{
	std::filesystem::current_path( path );
}

void Tree::Platform::ChangeCurrentDirectoryUTF8( std::string path )
{
	ChangeCurrentDirectoryPath( UTF8ToPath( path ) );
}

void Tree::Platform::InternalDebugLog( const char* file, int line, const char* function, const std::string_view text )
{
	std::string formatted = fmt::format( "[{}] [{}:{}:{}] {}\r\n", ENGINE_NAME, file, line, function, text );
	std::wstring wctext = utf8::utf8towchar( formatted );
	OutputDebugStringW( wctext.data() );
}

void Tree::Platform::FatalExit()
{
	exit( 1 );
}
