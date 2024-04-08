#pragma once

#include <string>

#include <SDL3/SDL.h>

#include "interfaces/iwindow.h"

namespace Tree
{
	class Window : public IWindow
	{
	public:
		Window() = delete;
		Window( std::string internalName, SDL_Window* sdlWindow );
		~Window();

		inline virtual std::string GetInternalName() const override
		{
			return m_internalName;
		}

	private:
		std::string m_internalName;
		SDL_Window* m_sdlWindow;
	};
}