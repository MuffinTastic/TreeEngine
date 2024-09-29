#pragma once

#include <string>
#include <functional>

#include "system.h"
#include "sys.h"

namespace Tree
{
	typedef Tree::ISystem* (__SysGroup_GetSystemFn)( std::string name );
	typedef void ( __SysGroup_SetSysStateFn )( Tree::SysState sysState );
	typedef void ( __SysGroup_ClearSysStateFn )( );

	enum ESysGroupTag
	{
		ESYSGROUPTAG_CORE,
		ESYSGROUPTAG_RENDERBACKEND
	};

	class SysGroup
	{
	public:
		SysGroup() = delete;
		SysGroup( Platform::PlatformModule* module, ESysGroupTag tag );

	public:
		Platform::PlatformModule* GetModule() const noexcept;
		ESysGroupTag GetTag() const noexcept;
		ISystem* GetSystem( std::string name );
		void SetSysState( SysState sysState );
		void ClearSysState();

	private:
		Platform::PlatformModule* m_module;
		ESysGroupTag m_tag;
		std::function<__SysGroup_GetSystemFn> m_getSystem;
		std::function<__SysGroup_SetSysStateFn> m_setSysState;
		std::function<__SysGroup_ClearSysStateFn> m_clearSysState;
	};
}

#define SYSGROUP_GETSYSTEM_FUNCNAME "__SysGroup_GetSystem"
#define SYSGROUP_SETSYSSTATE_FUNCNAME "__SysGroup_SetSysState"
#define SYSGROUP_CLEARSYSSTATE_FUNCNAME "__SysGroup_ClearSysState"

EXPORT  Tree::ISystem* __SysGroup_GetSystem( std::string name );
EXPORT  void __SysGroup_SetSysState( Tree::SysState sysState );
EXPORT  void __SysGroup_ClearSysState();

