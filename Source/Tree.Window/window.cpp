#include "window.h"

Tree::Window::Window( std::string internalName, SDL_Window* sdlWindow )
	: m_sdlWindow( sdlWindow )
{

}

Tree::Window::~Window()
{
	SDL_DestroyWindow( m_sdlWindow );
	m_sdlWindow = nullptr;
}
