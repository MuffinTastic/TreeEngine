#pragma once

#include <string>
#include <functional>

#include "system.h"

namespace Tree
{
	class Module;

	typedef Tree::ISystem* (__Module_GetSystemFn)( std::string name );
	typedef void ( __Module_UpdateSystemsFn )( Tree::Module* module );
	typedef void ( __Module_ResetSystemsFn )( );

	class Module
	{
	public:
		Module() = delete;
		Module( Platform::SharedLibrary* sharedLibrary );

	public:
		ISystem* GetSystem( std::string name );
		void UpdateSystems( Module* module );
		void ResetSystems();

	private:
		Platform::SharedLibrary* m_sharedLibrary;
		std::function<__Module_GetSystemFn> m_getSystem;
		std::function<__Module_UpdateSystemsFn> m_updateSystems;
		std::function<__Module_ResetSystemsFn> m_resetSystems;
	};
}

#define MODULE_GETSYSTEM_FUNCNAME "__Module_GetSystem"
#define MODULE_UPDATESYSTEMS_FUNCNAME "__Module_UpdateSystems"
#define MODULE_RESETSYSTEMS_FUNCNAME "__Module_ResetSystems"

EXPORT  Tree::ISystem* __Module_GetSystem( std::string name );
EXPORT  void __Module_UpdateSystems( Tree::Module* module );
EXPORT  void __Module_ResetSystems( Tree::Module* module );

