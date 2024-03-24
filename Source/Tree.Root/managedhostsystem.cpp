#include "interfaces/imanagedhostsystem.h"

#include <functional>
#include <filesystem>

#include <scope_guard/scope_guard.hpp>

#include <corehost/nethost.h>
#include <corehost/hostfxr.h>
#include <corehost/coreclr_delegates.h>

#include "interfaces/ilogsystem.h"
#include "Tree.NativeCommon/sys.h"

namespace Tree
{
	class ManagedHostSystem : public IManagedHostSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

	private:
		std::shared_ptr<ILogger> m_logger;

		Platform::SharedLibrary* m_hostfxrLibrary = nullptr;

		// I would use std::function here, but these typedefs are incompatible with it
		hostfxr_initialize_for_runtime_config_fn m_initRuntimeFn;
		//hostfxr_set_error_writer_fn m_setRuntimeErrorWriterFn;
		hostfxr_get_runtime_delegate_fn m_getRuntimeDelegateFn;
		hostfxr_set_runtime_property_value_fn m_setRuntimePropertyFn;
		hostfxr_close_fn m_closeFn;

		bool LoadHostFXR();
		void UnloadHostFXR();
		bool LoadNETCoreAssembly();
	};
}


REGISTER_TREE_SYSTEM( ManagedHostSystem, MANAGEDHOSTSYSTEM_NAME )


Tree::ESystemInitCode Tree::ManagedHostSystem::Startup()
{
	m_logger = Sys::Log()->CreateLogger( "ManagedHost" );

	if ( !LoadHostFXR() )
	{
		return ESYSTEMINIT_FAILURE;
	}



    return ESYSTEMINIT_SUCCESS;
}

void Tree::ManagedHostSystem::Shutdown()
{

	UnloadHostFXR();
}

bool Tree::ManagedHostSystem::LoadHostFXR()
{
	// I sure hope this is cross-platform

	char_t u16path[512];
	size_t bufferSize = sizeof( u16path ) / sizeof( char_t );

	if ( get_hostfxr_path( u16path, &bufferSize, nullptr ) != 0 )
	{
		m_logger->Error( "Couldn't find HostFXR" );
		return false;
	}

	std::filesystem::path hostfxrPath( u16path );
	m_hostfxrLibrary = Platform::LoadSharedLibrary( hostfxrPath );
	if ( !m_hostfxrLibrary )
	{
		m_logger->Error( "Couldn't load HostFXR at '{}'", Platform::PathToUTF8( hostfxrPath ) );
		return false;
	}

	m_initRuntimeFn = static_cast<hostfxr_initialize_for_runtime_config_fn>(
		Platform::GetSharedLibraryFunc(m_hostfxrLibrary, "hostfxr_initialize_for_runtime_config" ) );
	//m_setRuntimeErrorWriterFn = static_cast<hostfxr_set_error_writer_fn>(
	//	Platform::GetSharedLibraryFunc( m_hostfxrLibrary, "hostfxr_set_error_writer" ) );
	m_getRuntimeDelegateFn = static_cast<hostfxr_get_runtime_delegate_fn>(
		Platform::GetSharedLibraryFunc( m_hostfxrLibrary, "hostfxr_get_runtime_delegate" ) );
	m_setRuntimePropertyFn = static_cast<hostfxr_set_runtime_property_value_fn>(
		Platform::GetSharedLibraryFunc( m_hostfxrLibrary, "hostfxr_set_runtime_property_value" ) );
	m_closeFn = static_cast<hostfxr_close_fn>(
		Platform::GetSharedLibraryFunc( m_hostfxrLibrary, "hostfxr_close" ) );

	if ( !m_initRuntimeFn
		//|| !m_setRuntimeErrorWriterFn
		|| !m_getRuntimeDelegateFn
		|| !m_setRuntimePropertyFn
		|| !m_closeFn )
	{
		m_logger->Error( "Couldn't load all HostFXR functions" );
		Platform::UnloadSharedLibrary( m_hostfxrLibrary );
		return false;
	}

	return true;
}

void Tree::ManagedHostSystem::UnloadHostFXR()
{
	if ( m_hostfxrLibrary )
	{
		Platform::UnloadSharedLibrary( m_hostfxrLibrary );
		m_hostfxrLibrary = nullptr;
	}
}

bool Tree::ManagedHostSystem::LoadNETCoreAssembly()
{
	//auto contextGuard = sg::make_scope_guard( []
	//	{
	//		
	//	} );

	return true;
}
