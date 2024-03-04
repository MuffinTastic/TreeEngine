#pragma once

#include <string>
#include <unordered_map>

#include "platform.h"
#include "system.h"

namespace Tree
{
	class Module
	{
	public:
		static Module& GetCurrent();

	public:
		void RegisterSystem( SystemRegistry& registry );
		System* GetSystem( std::string name );

	private:
		std::unordered_map<std::string, System*> m_Systems;
	};

	Module& GetModuleFromSharedLibrary( Platform::SharedLibrary* sharedLibrary );
}

typedef Tree::Module& (*__GetModuleFunc)( );
#define GETMODULE_FUNCNAME "__SharedLibrary_GetModule"

EXPORT  Tree::Module& __SharedLibrary_GetModule();