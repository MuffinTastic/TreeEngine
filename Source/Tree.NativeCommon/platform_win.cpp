#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>

#include "unicode.h"


std::string Tree::Platform::GetExecutableFile()
{
	wchar_t path[MAX_PATH];
	GetModuleFileNameW( NULL, path, MAX_PATH );
	std::u16string_view u16path( reinterpret_cast<char16_t*>( path ) );
	return utf8::utf16to8( u16path );
}

std::string Tree::Platform::GetExecutableDirectory()
{
	std::filesystem::path path( GetExecutableFile() );
	return path.parent_path().string();
}

Tree::Platform::SharedLibrary* Tree::Platform::LoadSharedLibrary( std::string path )
{
	std::u16string u16path = utf8::utf8to16( path );
	wchar_t* wcpath = reinterpret_cast<wchar_t*>( u16path.data() );
	HMODULE library = LoadLibraryExW( wcpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	return reinterpret_cast<SharedLibrary*>( library );
}

void Tree::Platform::ChangeDirectory( std::string path )
{

}