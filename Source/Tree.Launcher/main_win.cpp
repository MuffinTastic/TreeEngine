#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include "Tree.NativeCommon/unicode.h"
#include "treemain.h"

// Common
int wmain( int argc, wchar_t** argv )
{
	std::vector<std::string> arguments;
	arguments.reserve( argc );

	for ( int i = 0; i < argc; i++ )
	{
		// Platform-specific code, this is okay
		std::u16string_view warg( reinterpret_cast<const char16_t*>( argv[i] ) );
		arguments.push_back( utf8::utf16to8( warg ) );
	}

	return Tree::TreeMain( arguments );
}


// Server
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow )
{
	int wargc;
	LPWSTR* wargv = CommandLineToArgvW( GetCommandLineW(), &wargc );

	return wmain( wargc, wargv );
}