#include "windowsystem.h"

#include "Tree.NativeCommon/sys.h"


REGISTER_TREE_SYSTEM( WindowSystem, WINDOWSYSTEM_NAME )


Tree::ESystemInitCode Tree::WindowSystem::Startup()
{
	m_logger = Sys::Log()->CreateLogger( "Window" );

	int sdlRet;

	sdlRet = SDL_InitSubSystem( SDL_INIT_VIDEO );
	if ( sdlRet < 0 )
	{
		m_logger->Error( "SDL video subsystem failed: {}", SDL_GetError() );
		return ESYSTEMINIT_FAILURE;
	}

	return ESYSTEMINIT_SUCCESS;
}

void Tree::WindowSystem::Shutdown()
{
	SDL_QuitSubSystem( SDL_INIT_VIDEO );

	// According to the SDL API this has to be called regardless of
	// whether or not all the subsystems have been quit individually.
	// WindowSystem is the last SDL-related system to Shutdown so we
	// can run this here.
	SDL_Quit();
}

Tree::IWindow* Tree::WindowSystem::CreateWindow( std::string internalName )
{
	return nullptr;
}
