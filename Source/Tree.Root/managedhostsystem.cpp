#include "interfaces/imanagedhostsystem.h"

#include <functional>
#include <filesystem>

#ifdef WINDOWS
#include <ShlObj_core.h>
#else
#include <dlfcn.h>
#endif

#include <scope_guard/scope_guard.hpp>

#include <corehost/hostfxr.h>
#include <corehost/coreclr_delegates.h>

#include "interfaces/ilogsystem.h"
#include "Tree.NativeCommon/sys.h"
#include "Tree.NativeCommon/platform.h"

#include "sap/GC.h"
#include "sap/SapArray.h"
#include "sap/Attribute.h"

#include "sap/Verify.h"
#include "sap/Core.h"
#include "sap/MessageLevel.h"
#include "sap/Assembly.h"
#include "sap/ManagedObject.h"
#include "sap/HostFXRErrorCodes.h"
#include "sap/SapManagedFunctions.h"
#include "sap/StringHelper.h"
#include "sap/TypeCache.h"

#define TREE_SAP_ASSEMBLY_NAME "Tree.Sap"
#define TREE_ENGINE_ASSEMBLY_NAME "Tree.Engine"
#define TREE_TRUNK_ASSEMBLY_NAME "Tree.Trunk"

// Managed shared libraries end in .dll no matter the platform
#define MANAGED_ASSEMBLY_EXT ".dll"
#define MANAGED_RUNTIMECFG_EXT ".runtimeconfig.json"

namespace Tree
{
	using ExceptionCallbackFn = std::function<void( std::string_view )>;

	class ManagedHostSystem : public IManagedHostSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

	public:
		std::shared_ptr<ILogger> m_logger;

	private:
		Sap::AssemblyLoadContext m_loadContext;

		void LoadEngineFunctions();

		void TrunkFunc();

	private:
		struct CoreCLRFunctions
		{
			hostfxr_set_error_writer_fn SetHostFXRErrorWriter = nullptr;
			hostfxr_initialize_for_runtime_config_fn InitHostFXRForRuntimeConfig = nullptr;
			hostfxr_get_runtime_delegate_fn GetRuntimeDelegate = nullptr;
			hostfxr_set_runtime_property_value_fn SetRuntimePropertyValue = nullptr;
			hostfxr_close_fn CloseHostFXR = nullptr;

			load_assembly_fn LoadAssembly = nullptr;
			get_function_pointer_fn GetFunctionPtr = nullptr;
		};
		static CoreCLRFunctions s_CoreCLRFunctions;

		Sap::AssemblyLoadContext CreateAssemblyLoadContext( std::string_view InName );
		void UnloadAssemblyLoadContext( Sap::AssemblyLoadContext& InLoadContext );

		void LoadHostFXR() const;
		bool InitializeSapManaged();
		void LoadSapFunctions();

		void LoadSapManagedAssembly( const std::filesystem::path& InAssemblyPath ) const;

		void* LoadSapManagedFunctionPtr( const SapChar* InTypeName, const SapChar* InMethodName, const SapChar* InDelegateType = SAP_UNMANAGED_CALLERS_ONLY ) const;

		template<typename TFunc>
		TFunc LoadSapManagedFunctionPtr( const SapChar* InTypeName, const SapChar* InMethodName, const SapChar* InDelegateType = SAP_UNMANAGED_CALLERS_ONLY ) const
		{
			return reinterpret_cast<TFunc>( LoadSapManagedFunctionPtr( InTypeName, InMethodName, InDelegateType ) );
		}

		void* m_HostFXRContext = nullptr;
		bool m_Initialized = false;
	};
}


REGISTER_TREE_SYSTEM( ManagedHostSystem, MANAGEDHOSTSYSTEM_NAME )

Tree::ManagedHostSystem::CoreCLRFunctions Tree::ManagedHostSystem::s_CoreCLRFunctions;

Tree::ESystemInitCode Tree::ManagedHostSystem::Startup()
{
	m_logger = Sys::Log()->CreateLogger( "ManagedHost" );

	SAP_VERIFY( !m_Initialized );

	LoadHostFXR();

	s_CoreCLRFunctions.SetHostFXRErrorWriter( []( const SapChar* InMessage )
		{
			auto message = Sap::StringHelper::ConvertWideToUtf8( InMessage );
			auto host = dynamic_cast<Tree::ManagedHostSystem*>( Sys::ManagedHost() );
			host->m_logger->Error( "{}", message );
		} );

	if ( !InitializeSapManaged() )
	{
		m_logger->Error( "Failed to initialize managed code" );
		return ESYSTEMINIT_FAILURE;
	}

	m_loadContext = CreateAssemblyLoadContext( "MainContext" );

	LoadEngineFunctions();

	TrunkFunc();

    return ESYSTEMINIT_SUCCESS;
}

void Tree::ManagedHostSystem::Shutdown()
{
	UnloadAssemblyLoadContext( m_loadContext );
	s_CoreCLRFunctions.CloseHostFXR( m_HostFXRContext );
}

void StringDemoClient( Tree::Sap::SapString str )
{
	Tree::Sys::Log()->Info( "vuhh {}", std::string( str ) );
}

void Tree::ManagedHostSystem::LoadEngineFunctions()
{
	std::filesystem::path enginePath = Platform::GetEngineDirectoryPath();
	auto assemblyPath = enginePath / TREE_ENGINE_ASSEMBLY_NAME MANAGED_ASSEMBLY_EXT;
	auto& assembly = m_loadContext.LoadAssembly( Platform::PathToUTF8( assemblyPath ) );

	assembly.AddInternalCall( "Tree.Engine.EngineTest", "TestFunc", &StringDemoClient );
	assembly.UploadInternalCalls();

	m_logger->Info( "Uploaded internal calls" );
}

void Tree::ManagedHostSystem::TrunkFunc()
{
	std::filesystem::path enginePath = Platform::GetEngineDirectoryPath();
	auto assemblyPath = enginePath / TREE_TRUNK_ASSEMBLY_NAME MANAGED_ASSEMBLY_EXT;
	auto& assembly = m_loadContext.LoadAssembly( Platform::PathToUTF8( assemblyPath ) );

	auto type = assembly.GetType( "Tree.Trunk.SapEntry" );
	type.InvokeStaticMethod( "Entry" );
}

Tree::Sap::AssemblyLoadContext Tree::ManagedHostSystem::CreateAssemblyLoadContext( std::string_view InName )
{
	Sap::ScopedSapString name = Sap::SapString::New( InName );
	Sap::AssemblyLoadContext alc;
	alc.m_ContextId = Sap::s_ManagedFunctions.CreateAssemblyLoadContextFptr( name );
	return alc;
}

void Tree::ManagedHostSystem::UnloadAssemblyLoadContext( Sap::AssemblyLoadContext& InLoadContext )
{
	Sap::s_ManagedFunctions.UnloadAssemblyLoadContextFptr( InLoadContext.m_ContextId );
	InLoadContext.m_ContextId = -1;
	InLoadContext.m_LoadedAssemblies.Clear();
}

std::filesystem::path GetHostFXRPath()
{
#ifdef WINDOWS
	std::filesystem::path basePath = "";

	// Find the Program Files folder
	TCHAR pf[MAX_PATH];
	SHGetSpecialFolderPath(
		nullptr,
		pf,
		CSIDL_PROGRAM_FILES,
		FALSE );

	basePath = pf;
	basePath /= "dotnet/host/fxr/";

	auto searchPaths = std::array
	{
		basePath
	};

#elif LINUX
	auto searchPaths = std::array
	{
		std::filesystem::path( "/usr/lib/dotnet/host/fxr/" ),
		std::filesystem::path( "/usr/share/dotnet/host/fxr/" ),
	};
#endif

	for ( const auto& path : searchPaths )
	{
		if ( !std::filesystem::exists( path ) )
			continue;

		for ( const auto& dir : std::filesystem::recursive_directory_iterator( path ) )
		{
			if ( !dir.is_directory() )
				continue;

			auto dirPath = dir.path().string();

			if ( dirPath.find( SAP_DOTNET_TARGET_VERSION_MAJOR_STR ) == std::string::npos )
				continue;

			auto res = dir / std::filesystem::path( SAP_HOSTFXR_NAME );
			SAP_VERIFY( std::filesystem::exists( res ) );
			return res;
		}
	}

	return "";
}

void Tree::ManagedHostSystem::LoadHostFXR() const
{
	// Retrieve the file path to the CoreCLR library
	auto hostfxrPath = GetHostFXRPath();

	// Load the CoreCLR library
	Platform::SharedLibrary* sharedLibrary = Platform::LoadSharedLibrary( hostfxrPath );

	SAP_VERIFY( sharedLibrary != nullptr );

	// Load CoreCLR functions
	s_CoreCLRFunctions.SetHostFXRErrorWriter = Platform::GetSharedLibraryFuncPtr<hostfxr_set_error_writer_fn>( sharedLibrary, "hostfxr_set_error_writer" );
	s_CoreCLRFunctions.InitHostFXRForRuntimeConfig = Platform::GetSharedLibraryFuncPtr<hostfxr_initialize_for_runtime_config_fn>( sharedLibrary, "hostfxr_initialize_for_runtime_config" );
	s_CoreCLRFunctions.GetRuntimeDelegate = Platform::GetSharedLibraryFuncPtr<hostfxr_get_runtime_delegate_fn>( sharedLibrary, "hostfxr_get_runtime_delegate" );
	s_CoreCLRFunctions.SetRuntimePropertyValue = Platform::GetSharedLibraryFuncPtr<hostfxr_set_runtime_property_value_fn>( sharedLibrary, "hostfxr_set_runtime_property_value" );
	s_CoreCLRFunctions.CloseHostFXR = Platform::GetSharedLibraryFuncPtr<hostfxr_close_fn>( sharedLibrary, "hostfxr_close" );
}

bool Tree::ManagedHostSystem::InitializeSapManaged()
{
	auto enginePath = Platform::GetEngineDirectoryPath();

	auto sapAssemblyPath = enginePath / TREE_SAP_ASSEMBLY_NAME MANAGED_ASSEMBLY_EXT;

	if ( !std::filesystem::exists( sapAssemblyPath ) )
	{
		m_logger->Error( "Failed to find {}", TREE_SAP_ASSEMBLY_NAME MANAGED_ASSEMBLY_EXT );
		return false;
	}

	//
	// Set CoreCLR properties and fetch assembly / function pointer delegates
	//

	auto runtimeConfigPath = enginePath / TREE_SAP_ASSEMBLY_NAME MANAGED_RUNTIMECFG_EXT;

	if ( !std::filesystem::exists( runtimeConfigPath ) )
	{
		m_logger->Error( "Failed to find {}", TREE_SAP_ASSEMBLY_NAME MANAGED_RUNTIMECFG_EXT );
		return false;
	}

	int status = s_CoreCLRFunctions.InitHostFXRForRuntimeConfig( runtimeConfigPath.c_str(), nullptr, &m_HostFXRContext );
	SAP_VERIFY( status == Sap::StatusCode::Success || status == Sap::StatusCode::Success_HostAlreadyInitialized || status == Sap::StatusCode::Success_DifferentRuntimeProperties );
	SAP_VERIFY( m_HostFXRContext != nullptr );

	status = s_CoreCLRFunctions.SetRuntimePropertyValue( m_HostFXRContext, SAP_STR( "APP_CONTEXT_BASE_DIRECTORY" ), enginePath.c_str() );
	SAP_VERIFY( status == Sap::StatusCode::Success );

	status = s_CoreCLRFunctions.SetRuntimePropertyValue( m_HostFXRContext, SAP_STR( "APP_PATHS" ), enginePath.c_str() );
	SAP_VERIFY( status == Sap::StatusCode::Success );

	status = s_CoreCLRFunctions.SetRuntimePropertyValue( m_HostFXRContext, SAP_STR( "APP_NI_PATHS" ), enginePath.c_str() );
	SAP_VERIFY( status == Sap::StatusCode::Success );

	status = s_CoreCLRFunctions.SetRuntimePropertyValue( m_HostFXRContext, SAP_STR( "NATIVE_DLL_SEARCH_DIRECTORIES" ), enginePath.c_str() );
	SAP_VERIFY( status == Sap::StatusCode::Success );

	status = s_CoreCLRFunctions.SetRuntimePropertyValue( m_HostFXRContext, SAP_STR( "PLATFORM_RESOURCE_ROOTS" ), enginePath.c_str() );
	SAP_VERIFY( status == Sap::StatusCode::Success );

	status = s_CoreCLRFunctions.GetRuntimeDelegate( m_HostFXRContext, hdt_load_assembly, (void**)&s_CoreCLRFunctions.LoadAssembly );
	SAP_VERIFY( status == Sap::StatusCode::Success );

	status = s_CoreCLRFunctions.GetRuntimeDelegate( m_HostFXRContext, hdt_get_function_pointer, (void**)&s_CoreCLRFunctions.GetFunctionPtr );
	SAP_VERIFY( status == Sap::StatusCode::Success );

	//
	// Load Tree.Sap and set functions
	//

	LoadSapManagedAssembly( sapAssemblyPath );

	using InitializeFn = void( * )(
		void( * )( Sap::SapString ),
		void( * )( Sap::SapString ),
		void( * )( Sap::SapString ),
		void( * )( Sap::SapString )
		);
	InitializeFn sapManagedEntryPoint = nullptr;
	sapManagedEntryPoint = LoadSapManagedFunctionPtr<InitializeFn>( SAP_STR( "Tree.Sap.ManagedHost, Tree.Sap" ), SAP_STR( "Initialize" ) );

	LoadSapFunctions();

	sapManagedEntryPoint(
		[]( Sap::SapString InMessage )
		{
			std::string message = InMessage;
			auto host = dynamic_cast<Tree::ManagedHostSystem*>( Sys::ManagedHost() );
			host->m_logger->Error( "C# exception caught: {}", message );
		},
		[]( Sap::SapString InMessage )
		{
			std::string message = InMessage;
			auto host = dynamic_cast<Tree::ManagedHostSystem*>( Sys::ManagedHost() );
			host->m_logger->Info( "{}", message );
		},
		[]( Sap::SapString InMessage )
		{
			std::string message = InMessage;
			auto host = dynamic_cast<Tree::ManagedHostSystem*>( Sys::ManagedHost() );
			host->m_logger->Warning( "{}", message );
		},
		[]( Sap::SapString InMessage )
		{
			std::string message = InMessage;
			auto host = dynamic_cast<Tree::ManagedHostSystem*>( Sys::ManagedHost() );
			host->m_logger->Error( "{}", message );
		}
		);

	return true;
}

void Tree::ManagedHostSystem::LoadSapFunctions()
{
	Sap::s_ManagedFunctions.CreateAssemblyLoadContextFptr = LoadSapManagedFunctionPtr<Sap::CreateAssemblyLoadContextFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Sap" ), SAP_STR( "CreateAssemblyLoadContext" ) );
	Sap::s_ManagedFunctions.UnloadAssemblyLoadContextFptr = LoadSapManagedFunctionPtr<Sap::UnloadAssemblyLoadContextFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Sap" ), SAP_STR( "UnloadAssemblyLoadContext" ) );
	Sap::s_ManagedFunctions.LoadManagedAssemblyFptr = LoadSapManagedFunctionPtr<Sap::LoadManagedAssemblyFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Sap" ), SAP_STR( "LoadAssembly" ) );
	Sap::s_ManagedFunctions.UnloadAssemblyLoadContextFptr = LoadSapManagedFunctionPtr<Sap::UnloadAssemblyLoadContextFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Sap" ), SAP_STR( "UnloadAssemblyLoadContext" ) );
	Sap::s_ManagedFunctions.GetLastLoadStatusFptr = LoadSapManagedFunctionPtr<Sap::GetLastLoadStatusFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Sap" ), SAP_STR( "GetLastLoadStatus" ) );
	Sap::s_ManagedFunctions.GetAssemblyNameFptr = LoadSapManagedFunctionPtr<Sap::GetAssemblyNameFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Sap" ), SAP_STR( "GetAssemblyName" ) );

	Sap::s_ManagedFunctions.GetAssemblyTypesFptr = LoadSapManagedFunctionPtr<Sap::GetAssemblyTypesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetAssemblyTypes" ) );
	Sap::s_ManagedFunctions.GetTypeIdFptr = LoadSapManagedFunctionPtr<Sap::GetTypeIdFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetTypeId" ) );
	Sap::s_ManagedFunctions.GetFullTypeNameFptr = LoadSapManagedFunctionPtr<Sap::GetFullTypeNameFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetFullTypeName" ) );
	Sap::s_ManagedFunctions.GetAssemblyQualifiedNameFptr = LoadSapManagedFunctionPtr<Sap::GetAssemblyQualifiedNameFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetAssemblyQualifiedName" ) );
	Sap::s_ManagedFunctions.GetBaseTypeFptr = LoadSapManagedFunctionPtr<Sap::GetBaseTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetBaseType" ) );
	Sap::s_ManagedFunctions.GetTypeSizeFptr = LoadSapManagedFunctionPtr<Sap::GetTypeSizeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetTypeSize" ) );
	Sap::s_ManagedFunctions.IsTypeSubclassOfFptr = LoadSapManagedFunctionPtr<Sap::IsTypeSubclassOfFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "IsTypeSubclassOf" ) );
	Sap::s_ManagedFunctions.IsTypeAssignableToFptr = LoadSapManagedFunctionPtr<Sap::IsTypeAssignableToFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "IsTypeAssignableTo" ) );
	Sap::s_ManagedFunctions.IsTypeAssignableFromFptr = LoadSapManagedFunctionPtr<Sap::IsTypeAssignableFromFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "IsTypeAssignableFrom" ) );
	Sap::s_ManagedFunctions.IsTypeSZArrayFptr = LoadSapManagedFunctionPtr<Sap::IsTypeSZArrayFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "IsTypeSZArray" ) );
	Sap::s_ManagedFunctions.GetElementTypeFptr = LoadSapManagedFunctionPtr<Sap::GetElementTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetElementType" ) );
	Sap::s_ManagedFunctions.GetTypeMethodsFptr = LoadSapManagedFunctionPtr<Sap::GetTypeMethodsFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetTypeMethods" ) );
	Sap::s_ManagedFunctions.GetTypeFieldsFptr = LoadSapManagedFunctionPtr<Sap::GetTypeFieldsFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetTypeFields" ) );
	Sap::s_ManagedFunctions.GetTypePropertiesFptr = LoadSapManagedFunctionPtr<Sap::GetTypePropertiesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetTypeProperties" ) );
	Sap::s_ManagedFunctions.HasTypeAttributeFptr = LoadSapManagedFunctionPtr<Sap::HasTypeAttributeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "HasTypeAttribute" ) );
	Sap::s_ManagedFunctions.GetTypeAttributesFptr = LoadSapManagedFunctionPtr<Sap::GetTypeAttributesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetTypeAttributes" ) );
	Sap::s_ManagedFunctions.GetTypeManagedTypeFptr = LoadSapManagedFunctionPtr<Sap::GetTypeManagedTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetTypeManagedType" ) );
	Sap::s_ManagedFunctions.InvokeStaticMethodFptr = LoadSapManagedFunctionPtr<Sap::InvokeStaticMethodFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Sap" ), SAP_STR( "InvokeStaticMethod" ) );
	Sap::s_ManagedFunctions.InvokeStaticMethodRetFptr = LoadSapManagedFunctionPtr<Sap::InvokeStaticMethodRetFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Sap" ), SAP_STR( "InvokeStaticMethodRet" ) );

	Sap::s_ManagedFunctions.GetMethodInfoNameFptr = LoadSapManagedFunctionPtr<Sap::GetMethodInfoNameFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetMethodInfoName" ) );
	Sap::s_ManagedFunctions.GetMethodInfoReturnTypeFptr = LoadSapManagedFunctionPtr<Sap::GetMethodInfoReturnTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetMethodInfoReturnType" ) );
	Sap::s_ManagedFunctions.GetMethodInfoParameterTypesFptr = LoadSapManagedFunctionPtr<Sap::GetMethodInfoParameterTypesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetMethodInfoParameterTypes" ) );
	Sap::s_ManagedFunctions.GetMethodInfoAccessibilityFptr = LoadSapManagedFunctionPtr<Sap::GetMethodInfoAccessibilityFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetMethodInfoAccessibility" ) );
	Sap::s_ManagedFunctions.GetMethodInfoAttributesFptr = LoadSapManagedFunctionPtr<Sap::GetMethodInfoAttributesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetMethodInfoAttributes" ) );

	Sap::s_ManagedFunctions.GetFieldInfoNameFptr = LoadSapManagedFunctionPtr<Sap::GetFieldInfoNameFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetFieldInfoName" ) );
	Sap::s_ManagedFunctions.GetFieldInfoTypeFptr = LoadSapManagedFunctionPtr<Sap::GetFieldInfoTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetFieldInfoType" ) );
	Sap::s_ManagedFunctions.GetFieldInfoAccessibilityFptr = LoadSapManagedFunctionPtr<Sap::GetFieldInfoAccessibilityFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetFieldInfoAccessibility" ) );
	Sap::s_ManagedFunctions.GetFieldInfoAttributesFptr = LoadSapManagedFunctionPtr<Sap::GetFieldInfoAttributesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetFieldInfoAttributes" ) );

	Sap::s_ManagedFunctions.GetPropertyInfoNameFptr = LoadSapManagedFunctionPtr<Sap::GetPropertyInfoNameFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetPropertyInfoName" ) );
	Sap::s_ManagedFunctions.GetPropertyInfoTypeFptr = LoadSapManagedFunctionPtr<Sap::GetPropertyInfoTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetPropertyInfoType" ) );
	Sap::s_ManagedFunctions.GetPropertyInfoAttributesFptr = LoadSapManagedFunctionPtr<Sap::GetPropertyInfoAttributesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetPropertyInfoAttributes" ) );

	Sap::s_ManagedFunctions.GetAttributeFieldValueFptr = LoadSapManagedFunctionPtr<Sap::GetAttributeFieldValueFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetAttributeFieldValue" ) );
	Sap::s_ManagedFunctions.GetAttributeTypeFptr = LoadSapManagedFunctionPtr<Sap::GetAttributeTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Sap" ), SAP_STR( "GetAttributeType" ) );

	Sap::s_ManagedFunctions.SetInternalCallsFptr = LoadSapManagedFunctionPtr<Sap::SetInternalCallsFn>( SAP_STR( "Tree.Sap.Interop.InternalCallsManager, Tree.Sap" ), SAP_STR( "SetInternalCalls" ) );
	Sap::s_ManagedFunctions.CreateObjectFptr = LoadSapManagedFunctionPtr<Sap::CreateObjectFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Sap" ), SAP_STR( "CreateObject" ) );
	Sap::s_ManagedFunctions.InvokeMethodFptr = LoadSapManagedFunctionPtr<Sap::InvokeMethodFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Sap" ), SAP_STR( "InvokeMethod" ) );
	Sap::s_ManagedFunctions.InvokeMethodRetFptr = LoadSapManagedFunctionPtr<Sap::InvokeMethodRetFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Sap" ), SAP_STR( "InvokeMethodRet" ) );
	Sap::s_ManagedFunctions.SetFieldValueFptr = LoadSapManagedFunctionPtr<Sap::SetFieldValueFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Sap" ), SAP_STR( "SetFieldValue" ) );
	Sap::s_ManagedFunctions.GetFieldValueFptr = LoadSapManagedFunctionPtr<Sap::GetFieldValueFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Sap" ), SAP_STR( "GetFieldValue" ) );
	Sap::s_ManagedFunctions.SetPropertyValueFptr = LoadSapManagedFunctionPtr<Sap::SetFieldValueFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Sap" ), SAP_STR( "SetPropertyValue" ) );
	Sap::s_ManagedFunctions.GetPropertyValueFptr = LoadSapManagedFunctionPtr<Sap::GetFieldValueFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Sap" ), SAP_STR( "GetPropertyValue" ) );
	Sap::s_ManagedFunctions.DestroyObjectFptr = LoadSapManagedFunctionPtr<Sap::DestroyObjectFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Sap" ), SAP_STR( "DestroyObject" ) );
	Sap::s_ManagedFunctions.CollectGarbageFptr = LoadSapManagedFunctionPtr<Sap::CollectGarbageFn>( SAP_STR( "Tree.Sap.GarbageCollector, Tree.Sap" ), SAP_STR( "CollectGarbage" ) );
	Sap::s_ManagedFunctions.WaitForPendingFinalizersFptr = LoadSapManagedFunctionPtr<Sap::WaitForPendingFinalizersFn>( SAP_STR( "Tree.Sap.GarbageCollector, Tree.Sap" ), SAP_STR( "WaitForPendingFinalizers" ) );
}

void Tree::ManagedHostSystem::LoadSapManagedAssembly( const std::filesystem::path& InAssemblyPath ) const
{
	int status = s_CoreCLRFunctions.LoadAssembly( InAssemblyPath.c_str(), nullptr, nullptr );
	SAP_VERIFY( status == Sap::StatusCode::Success );
}

void* Tree::ManagedHostSystem::LoadSapManagedFunctionPtr( const SapChar* InTypeName, const SapChar* InMethodName, const SapChar* InDelegateType ) const
{
	void* funcPtr = nullptr;
	int status = s_CoreCLRFunctions.GetFunctionPtr( InTypeName, InMethodName, InDelegateType, nullptr, nullptr, &funcPtr );
	SAP_VERIFY( status == Sap::StatusCode::Success && funcPtr != nullptr );
	return funcPtr;
}