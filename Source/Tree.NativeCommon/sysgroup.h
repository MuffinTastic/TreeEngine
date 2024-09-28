#pragma once

#include <string>
#include <functional>

#include "system.h"

namespace Tree
{
	class SysGroup;

	typedef Tree::ISystem* (__SysGroup_GetSystemFn)( std::string name );
	typedef void ( __SysGroup_UpdateSystemsFn )( Tree::SysGroup* sysGroup );
	typedef void ( __SysGroup_ResetSystemsFn )( );

	class SysGroup
	{
	public:
		SysGroup() = delete;
		SysGroup( Platform::SharedLibrary* sharedLibrary );

	public:
		ISystem* GetSystem( std::string name );
		void UpdateSystems( SysGroup* sysGroup );
		void ResetSystems();

	private:
		Platform::SharedLibrary* m_sharedLibrary;
		std::function<__SysGroup_GetSystemFn> m_getSystem;
		std::function<__SysGroup_UpdateSystemsFn> m_updateSystems;
		std::function<__SysGroup_ResetSystemsFn> m_resetSystems;
	};
}

#define SYSGROUP_GETSYSTEM_FUNCNAME "__SysGroup_GetSystem"
#define SYSGROUP_UPDATESYSTEMS_FUNCNAME "__SysGroup_UpdateSystems"
#define SYSGROUP_RESETSYSTEMS_FUNCNAME "__SysGroup_ResetSystems"

EXPORT  Tree::ISystem* __SysGroup_GetSystem( std::string name );
EXPORT  void __SysGroup_UpdateSystems( Tree::SysGroup* sysGroup );
EXPORT  void __SysGroup_ResetSystems( Tree::SysGroup* sysGroup );

