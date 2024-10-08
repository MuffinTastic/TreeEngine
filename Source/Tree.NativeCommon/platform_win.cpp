#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include "unicode.h"

#include "Tree.NativeCommon/defs.h"


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

Tree::Platform::PlatformModule* Tree::Platform::GetExecutableModule()
{
	HMODULE handle = GetModuleHandleW( NULL );
	return reinterpret_cast<PlatformModule*>( handle );
}

Tree::Platform::PlatformModule* Tree::Platform::LoadModule( std::filesystem::path path )
{
	std::wstring wpath = path.wstring();
	HMODULE handle = LoadLibraryExW( wpath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	return reinterpret_cast<PlatformModule*>( handle );
}

void Tree::Platform::UnloadModule( PlatformModule* module )
{
	HMODULE handle = reinterpret_cast<HMODULE>( module );
	FreeLibrary( handle );
}

void* Tree::Platform::GetModuleFunc( PlatformModule* module, std::string name )
{
	HMODULE handle = reinterpret_cast<HMODULE>( module );
	const char* cname = name.c_str();
	FARPROC address = GetProcAddress( handle, cname );
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
