#include "treemain.h"

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#endif

#include <iostream>

#include "Tree.NativeCommon/unicode.h"
#include "Tree.NativeCommon/platform.h"

#ifdef WINDOWS
void OpenWindowsConsole()
{
    AllocConsole();
    //(void)freopen( "CONIN$", "r", stdin );
    //(void)freopen( "CONOUT$", "w", stdout );
    //(void)freopen( "CONOUT$", "w", stderr );
    

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
}

#endif

int Tree::TreeMain( std::vector<std::string> arguments )
{
#ifdef WINDOWS
    #if DEBUG

        #ifdef CLIENT
            OpenWindowsConsole();
        #endif

        SetConsoleOutputCP( CP_UTF8 );
        // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
        setvbuf( stdout, nullptr, _IOFBF, 1000 );

    #endif // DEBUG
#endif // WINDOWS

#ifdef CLIENT
    std::cout << "Client" << std::endl;
#elif DEDICATED_SERVER
    std::cout << "Server" << std::endl;
#endif

    std::cout << "Exe directory: " << Platform::GetExecutableDirectory() << std::endl;

    //std::cout << "Exe path: " << GetExecutablePath() << std::endl;

    for ( auto it = arguments.begin(); it < arguments.end(); it++ )
    {
        std::cout << *it << std::endl;
        //OutputDebugStringA( (*it).data() );

        std::u16string warg = utf8::utf8to16( *it + "\n" );
        OutputDebugStringW( reinterpret_cast<wchar_t*>( warg.data() ) );
    }

    return 0;
}