#include "treemain.h"

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#endif

#include <iostream>
#include <filesystem>

#include "Tree.NativeCommon/unicode.h"
#include "Tree.NativeCommon/platform.h"
#include "Tree.NativeCommon/module.h"

#include "Tree.Root/interfaces/ienginesystem.h"
#include "Tree.Root/interfaces/itestsystem.h"

#include "Tree.NativeCommon/sys.h"

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

    std::cout << "Exe directory: " << basepath << std::endl;

    using fspath = std::filesystem::path;
    fspath rootlib = fspath( basepath ) / "Engine" / "Tree.Root.dll";
    Platform::SharedLibrary* library = Platform::LoadSharedLibrary( rootlib.string() );

    Module rootModule( library );
    rootModule.UpdateSystems( &rootModule );

    IEngineSystem* engineSystem = dynamic_cast<IEngineSystem*>( rootModule.GetSystem( ENGINESYSTEM_NAME ) );
    std::cout << engineSystem << std::endl;

    ITestSystem* testSystem = dynamic_cast<ITestSystem*>( rootModule.GetSystem( TESTSYSTEM_NAME ) );
    testSystem->RootSpecific();

    std::cout << Sys::Engine() << std::endl;

    return 0;
}