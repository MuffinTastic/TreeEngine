#include "treemain.h"

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#endif

#include <iostream>
#include <filesystem>

#include <scope_guard/scope_guard.hpp>

#include "Tree.NativeCommon/unicode.h"
#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/module.h"

#include "Tree.Root/interfaces/ienginesystem.h"
#include "Tree.Root/interfaces/itestsystem.h"

#include "Tree.NativeCommon/sys.h"

#include "modulemanager.h"

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

    std::string basepath = Platform::GetExecutableDirectory();
    Platform::ChangeCurrentDirectory( basepath );

    {
        ModuleManager::Instance().LoadModules( {
            "Tree.Root"
        } );

        auto moduleGuard = sg::make_scope_guard( [] {
            ModuleManager::Instance().UnloadModules();
        } );


        if ( Sys::Engine()->Startup() != ESYSTEMINIT_SUCCESS )
        {
            return 1;
        }

        auto engineGuard = sg::make_scope_guard( [] {
            Sys::Engine()->Shutdown();
        } );



    }

    return 0;
}