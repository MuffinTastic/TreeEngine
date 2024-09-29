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
#include "Tree.NativeCommon/sysgroup.h"
#include "Tree.NativeCommon/sys.h"
#include "Tree.NativeCommon/domain.h"

#include "Tree.Root/interfaces/ienginesystem.h"
#include "Tree.Root/interfaces/itestsystem.h"

#include "sysgroupmanager.h"

#if !DEDICATED_SERVER
#if WINDOWS
void OpenWindowsConsole()
{
	AllocConsole();
	(void)freopen( "CONIN$", "r", stdin );
	(void)freopen( "CONOUT$", "w", stdout );
	(void)freopen( "CONOUT$", "w", stderr );
}

#endif // WINDOWS

void ShowErrorBox( std::string text )
{
#ifdef WINDOWS
	std::u16string u16text = utf8::utf8to16( text );
	const wchar_t* wctext = reinterpret_cast<const wchar_t*>( u16text.data() );
	MessageBoxW( nullptr, wctext, L"Tree Engine - Fatal Error", MB_ICONERROR );
#elif LINUX

#endif // LINUX
}
#endif // !DEDICATED_SERVER

int Tree::TreeMain( std::vector<std::string> arguments )
{
	// We don't want the executable path this way,
	// we'll grab it ourselves through other means...
	arguments.erase( arguments.begin() );

#ifdef GUI_ENABLED
#if DEBUG
#if WINDOWS
	OpenWindowsConsole();
#endif // WINDOWS
#endif // DEBUG
#endif // !DEDICATED_SERVER

#if WINDOWS
	SetConsoleOutputCP( CP_UTF8 );
	// Enable buffering to prevent VS from chopping up UTF-8 byte sequences
	setvbuf( stdout, nullptr, _IOFBF, 1000 );
#endif

#ifdef GAME
	std::cout << "Game" << std::endl;
#elif DEDICATED_SERVER
	std::cout << "Server" << std::endl;
#endif

	Platform::ChangeCurrentDirectoryPath( Platform::GetExecutableDirectoryPath() );

	{
		SysGroupManager::Bootstrap();

		//
		// Load all the necessary systems for the current domain
		//
		std::vector<std::tuple<std::string, ESysGroupTag>> modulesToLoad = {
			{"Tree.Root", ESYSGROUPTAG_CORE},
#ifdef GUI_ENABLED
			{"Tree.Window", ESYSGROUPTAG_CORE},
#endif
		};

		if ( Sys::SysGroups()->LoadGroupsFrom( modulesToLoad ) != ESYSGROUPLOAD_SUCCESS )
		{
			Platform::DebugLog( "Couldn't load engine systems - quitting." );
#ifdef GUI_ENABLED
			ShowErrorBox( "Couldn't load engine systems - please capture stdout or look at a platform debug log to debug." );
#endif
			return TREEMAIN_FAILURE_MODULE;
		}

		auto sysGroupGuard = sg::make_scope_guard( []
			{
				Sys::SysGroups()->Shutdown();
			} );



		//
		// SysGroups have been loaded. Start engine dependencies.
		//

		// -- CmdLine --

		Sys::CmdLine()->ProcessArguments( arguments );

		// -- Log --

		if ( Sys::Log()->Startup() != ESYSTEMINIT_SUCCESS )
		{
			return TREEMAIN_FAILURE_SYSTEM;
		}

		auto logGuard = sg::make_scope_guard( []
			{
				Sys::Log()->Shutdown();
			} );

#ifdef GUI_ENABLED
		// -- Window --

		if ( Sys::Window()->Startup() != ESYSTEMINIT_SUCCESS )
		{
			return TREEMAIN_FAILURE_SYSTEM;
		}

		auto windowGuard = sg::make_scope_guard( []
			{
				Sys::Window()->Shutdown();
			} );
#endif

		// -- ManagedHost --

		if ( Sys::ManagedHost()->Startup() != ESYSTEMINIT_SUCCESS )
		{
			return TREEMAIN_FAILURE_SYSTEM;
		}

		auto managedHostGuard = sg::make_scope_guard( []
			{
				Sys::ManagedHost()->Shutdown();
			} );

		// -- Engine --

		EngineStartupConfig config;

#if EDITOR
		config.domain = EDOMAIN_EDITOR;
#elif GAME
		config.domain = EDOMAIN_GAME;
#elif DEDICATED_SERVER
		config.domain = EDOMAIN_DEDICATED_SERVER;
#endif

		Sys::Engine()->SetStartupConfig( config );

		if ( Sys::Engine()->Startup() != ESYSTEMINIT_SUCCESS )
		{
			return TREEMAIN_FAILURE_SYSTEM;
		}

		auto engineGuard = sg::make_scope_guard( []
			{
				Sys::Engine()->Shutdown();
			} );


		Sys::Engine()->Run();
	}

	return TREEMAIN_SUCCESS;
}