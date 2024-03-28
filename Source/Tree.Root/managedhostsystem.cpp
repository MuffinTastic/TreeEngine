#include "interfaces/imanagedhostsystem.h"

#include <functional>
#include <filesystem>

#include <scope_guard/scope_guard.hpp>

#include <corehost/nethost.h>
#include <corehost/hostfxr.h>
#include <corehost/coreclr_delegates.h>

#include "interfaces/ilogsystem.h"
#include "Tree.NativeCommon/sys.h"

#include "sap/HostInstance.h"
#include "sap/GC.h"
#include "sap/SapArray.h"
#include "sap/Attribute.h"

#define TREE_TRUNK_ASSEMBLY_NAME "Tree.Trunk"

// Managed shared libraries end in .dll no matter the platform
#define MANAGED_ASSEMBLY_EXT ".dll"
#define MANAGED_RUNTIMECFG_EXT ".runtimeconfig.json"

// https://github.com/dotnet/runtime/blob/main/docs/design/features/host-error-codes.md
enum HostFXRStatusCode
{
	// Success codes
	Success = 0,
	Success_HostAlreadyInitialized = 0x00000001,
	Success_DifferentRuntimeProperties = 0x00000002,

	// Failure codes
	InvalidArgFailure = 0x80008081,
	CoreHostLibLoadFailure = 0x80008082,
	CoreHostLibMissingFailure = 0x80008083,
	CoreHostEntryPointFailure = 0x80008084,
	CoreHostCurHostFindFailure = 0x80008085,
	CoreClrResolveFailure = 0x80008087,
	CoreClrBindFailure = 0x80008088,
	CoreClrInitFailure = 0x80008089,
	CoreClrExeFailure = 0x8000808a,
	ResolverInitFailure = 0x8000808b,
	ResolverResolveFailure = 0x8000808c,
	LibHostCurExeFindFailure = 0x8000808d,
	LibHostInitFailure = 0x8000808e,
	LibHostExecModeFailure = 0x80008090,
	LibHostSdkFindFailure = 0x80008091,
	LibHostInvalidArgs = 0x80008092,
	InvalidConfigFile = 0x80008093,
	AppArgNotRunnable = 0x80008094,
	AppHostExeNotBoundFailure = 0x80008095,
	FrameworkMissingFailure = 0x80008096,
	HostApiFailed = 0x80008097,
	HostApiBufferTooSmall = 0x80008098,
	LibHostUnknownCommand = 0x80008099,
	LibHostAppRootFindFailure = 0x8000809a,
	SdkResolverResolveFailure = 0x8000809b,
	FrameworkCompatFailure = 0x8000809c,
	FrameworkCompatRetry = 0x8000809d,
	BundleExtractionFailure = 0x8000809f,
	BundleExtractionIOError = 0x800080a0,
	LibHostDuplicateProperty = 0x800080a1,
	HostApiUnsupportedVersion = 0x800080a2,
	HostInvalidState = 0x800080a3,
	HostPropertyNotFound = 0x800080a4,
	CoreHostIncompatibleConfig = 0x800080a5,
	HostApiUnsupportedScenario = 0x800080a6,
	HostFeatureDisabled = 0x800080a7,
};

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

		load_assembly_fn m_loadAssemblyFn;
		get_function_pointer_fn m_getFunctionPointerFn;

		bool LoadHostFXR();
		void UnloadHostFXR();

		bool LoadNETCore();

		bool LoadTreeTrunk();
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

	if ( !LoadNETCore() )
	{
		UnloadHostFXR();
		return ESYSTEMINIT_FAILURE;
	}

	m_logger->Info( "Loaded .NET Core" );

	if ( !LoadTreeTrunk() )
	{
		UnloadHostFXR();
		return ESYSTEMINIT_FAILURE;
	}

	m_logger->Info( "Loaded Tree.Trunk" );

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

bool Tree::ManagedHostSystem::LoadNETCore()
{
	/*
	HostFXRStatusCode code;

	std::filesystem::path enginePath = Platform::GetEngineDirectoryPath();

	// Though this is for the entire runtime, we just piggyback off of Tree.Trunk's autogenerated config
	std::filesystem::path configPath = ( enginePath / TREE_TRUNK_ASSEMBLY_NAME MANAGED_RUNTIMECFG_EXT );

	hostfxr_handle context = nullptr;
	code = static_cast<HostFXRStatusCode>( m_initRuntimeFn( configPath.c_str(), nullptr, &context ) );
	if ( code != Success || context == nullptr )
	{
		m_logger->Error( "Failed to init runtime with config '{}', error code {}", Platform::PathToUTF8( configPath ), static_cast<int>( code ) );
		return false;
	}

	auto contextGuard = sg::make_scope_guard( [=]
		{
			m_closeFn( context );
		} );

#define SAP_SET_RUNTIME_PROPERTY( propertyName, value ) \
		if ( ( code = static_cast<HostFXRStatusCode>( m_setRuntimePropertyFn( context, (propertyName), (value) ) ) ) != Success ) \
		{ \
			m_logger->Error( "Failed to set .NET runtime property '{}' to '{}', error code {}", \
				Sap::StringHelper::ConvertWideToUtf8(propertyName), Sap::StringHelper::ConvertWideToUtf8(value), static_cast<int>( code ) ); \
			return false; \
		}

	SAP_SET_RUNTIME_PROPERTY( SAP_STR("APP_CONTEXT_BASE_DIRECTORY"), enginePath.c_str() );
	SAP_SET_RUNTIME_PROPERTY( SAP_STR("APP_PATHS"), enginePath.c_str() );
	SAP_SET_RUNTIME_PROPERTY( SAP_STR("APP_NI_PATHS"), enginePath.c_str() );
	SAP_SET_RUNTIME_PROPERTY( SAP_STR("NATIVE_DLL_SEARCH_DIRECTORIES"), enginePath.c_str() );
	SAP_SET_RUNTIME_PROPERTY( SAP_STR("PLATFORM_RESOURCE_ROOTS"), enginePath.c_str() );

#undef SAP_SET_RUNTIME_PROPERTY

	void* loadAssemblyFn = nullptr;
	void* getFunctionPointerFn = nullptr;

	code = static_cast<HostFXRStatusCode>( m_getRuntimeDelegateFn( context, hdt_load_assembly, &loadAssemblyFn ) );
	if ( code != Success || loadAssemblyFn == nullptr )
	{
		m_logger->Error( "Couldn't get .NET Runtime load_assembly delegate" );
		return false;
	}

	code = static_cast<HostFXRStatusCode>( m_getRuntimeDelegateFn( context, hdt_get_function_pointer, &getFunctionPointerFn ) );
	if ( code != Success || getFunctionPointerFn == nullptr )
	{
		m_logger->Error( "Couldn't get .NET Runtime get_function_pointer delegate" );
		return false;
	}

	m_loadAssemblyFn = static_cast<load_assembly_fn>( loadAssemblyFn );
	m_getFunctionPointerFn = static_cast<get_function_pointer_fn>( getFunctionPointerFn );

	return true;
	*/

	return true;
}

void TestPRintYeaaggh( Tree::Sap::SapString str )
{
	std::string contensts = str;
	Tree::Sys::Log()->Info( "This is the thing: {}", contensts );
}

void TheArray( Tree::Sap::SapArray<float> floats )
{
	for ( auto it = floats.begin(); it != floats.end(); ++it )
	{
		Tree::Sys::Log()->Info( "Float: {}", *it );
	}
}

bool Tree::ManagedHostSystem::LoadTreeTrunk()
{
	std::filesystem::path enginePath = Platform::GetEngineDirectoryPath();
	std::filesystem::path assemblyPath = ( enginePath / TREE_TRUNK_ASSEMBLY_NAME MANAGED_ASSEMBLY_EXT );

	m_loadAssemblyFn( assemblyPath.c_str(), nullptr, nullptr );

	void* testFn = nullptr;

	HostFXRStatusCode code = static_cast<HostFXRStatusCode>( m_getFunctionPointerFn(
		SAP_STR("Tree.Trunk.SapEntry, Tree.Trunk"),
		SAP_STR("Entry"),
		UNMANAGEDCALLERSONLY_METHOD,
		nullptr, nullptr,
		&testFn
	) );

	if ( code != Success || testFn == nullptr )
	{
		m_logger->Error( "Couldn't get Tree.Trunk entrypoint" );
		return false;
	}

	std::vector<void*> funcs;
	funcs.push_back( &TestPRintYeaaggh );
	funcs.push_back( &TheArray );

	auto saparray = Sap::SapArray<void*>::New( funcs );

	auto fn = static_cast<void ( * )( Sap::SapArray<void*> )>( testFn );

	fn( saparray );

	return true;
}
