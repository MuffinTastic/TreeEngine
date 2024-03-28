#include "HostInstance.h"

#include <filesystem>

#ifdef WINDOWS
#include <ShlObj_core.h>
#else
#include <dlfcn.h>
#endif

#include <corehost/hostfxr.h>
#include <corehost/coreclr_delegates.h>

#include "Verify.h"
#include "HostFXRErrorCodes.h"
#include "SapManagedFunctions.h"
#include "StringHelper.h"
#include "TypeCache.h"

namespace Tree
{
	namespace Sap
	{
		struct CoreCLRFunctions
		{
			hostfxr_set_error_writer_fn SetHostFXRErrorWriter = nullptr;
			hostfxr_initialize_for_runtime_config_fn InitHostFXRForRuntimeConfig = nullptr;
			hostfxr_get_runtime_delegate_fn GetRuntimeDelegate = nullptr;
			hostfxr_close_fn CloseHostFXR = nullptr;
			load_assembly_and_get_function_pointer_fn GetManagedFunctionPtr = nullptr;
		};
		static CoreCLRFunctions s_CoreCLRFunctions;

		MessageCallbackFn MessageCallback = nullptr;
		MessageLevel MessageFilter;
		ExceptionCallbackFn ExceptionCallback = nullptr;

		void DefaultMessageCallback( std::string_view InMessage, MessageLevel InLevel )
		{
			const char* level = "";

			switch ( InLevel )
			{
			case MessageLevel::Info:
				level = "Info";
				break;
			case MessageLevel::Warning:
				level = "Warn";
				break;
			case MessageLevel::Error:
				level = "Error";
				break;
			}

			// TODO: Fix callback
		}

		bool HostInstance::Initialize( HostSettings InSettings )
		{
			SAP_VERIFY( !m_Initialized );

			LoadHostFXR();

			// Setup settings
			m_Settings = std::move( InSettings );

			if ( !m_Settings.MessageCallback )
				m_Settings.MessageCallback = DefaultMessageCallback;
			MessageCallback = m_Settings.MessageCallback;
			MessageFilter = m_Settings.MessageFilter;

			s_CoreCLRFunctions.SetHostFXRErrorWriter( []( const SapChar* InMessage )
				{
					auto message = StringHelper::ConvertWideToUtf8( InMessage );
					MessageCallback( message, MessageLevel::Error );
				} );

			m_SapManagedAssemblyPath = std::filesystem::path( m_Settings.SapDirectory ) / "Tree.Trunk.dll";

			if ( !std::filesystem::exists( m_SapManagedAssemblyPath ) )
			{
				MessageCallback( "Failed to find Tree.Trunk.dll", MessageLevel::Error );
				return false;
			}

			m_Initialized = InitializeSapManaged();

			return m_Initialized;
		}

		void HostInstance::Shutdown()
		{
			s_CoreCLRFunctions.CloseHostFXR( m_HostFXRContext );
		}

		AssemblyLoadContext HostInstance::CreateAssemblyLoadContext( std::string_view InName )
		{
			ScopedSapString name = SapString::New( InName );
			AssemblyLoadContext alc;
			alc.m_ContextId = s_ManagedFunctions.CreateAssemblyLoadContextFptr( name );
			alc.m_Host = this;
			return alc;
		}

		void HostInstance::UnloadAssemblyLoadContext( AssemblyLoadContext& InLoadContext )
		{
			s_ManagedFunctions.UnloadAssemblyLoadContextFptr( InLoadContext.m_ContextId );
			InLoadContext.m_ContextId = -1;
			InLoadContext.m_LoadedAssemblies.Clear();
		}

#ifdef _WIN32
		template <typename TFunc>
		TFunc LoadFunctionPtr( void* InLibraryHandle, const char* InFunctionName )
		{
			auto result = (TFunc)GetProcAddress( (HMODULE)InLibraryHandle, InFunctionName );
			SAP_VERIFY( result );
			return result;
		}
#else
		template <typename TFunc>
		TFunc LoadFunctionPtr( void* InLibraryHandle, const char* InFunctionName )
		{
			auto result = (TFunc)dlsym( InLibraryHandle, InFunctionName );
			SAP_VERIFY( result );
			return result;
		}
#endif

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

		void HostInstance::LoadHostFXR() const
		{
			// TODO: 

			// Retrieve the file path to the CoreCLR library
			auto hostfxrPath = GetHostFXRPath();

			// Load the CoreCLR library
			void* libraryHandle = nullptr;

#ifdef WINDOWS
#ifdef SAP_WIDE_CHARS
			libraryHandle = LoadLibraryW( hostfxrPath.c_str() );
#else
			libraryHandle = LoadLibraryA( hostfxrPath.string().c_str() );
#endif
#else
			libraryHandle = dlopen( hostfxrPath.string().data(), RTLD_NOW | RTLD_GLOBAL );
#endif

			SAP_VERIFY( libraryHandle != nullptr );

			// Load CoreCLR functions
			s_CoreCLRFunctions.SetHostFXRErrorWriter = LoadFunctionPtr<hostfxr_set_error_writer_fn>( libraryHandle, "hostfxr_set_error_writer" );
			s_CoreCLRFunctions.InitHostFXRForRuntimeConfig = LoadFunctionPtr<hostfxr_initialize_for_runtime_config_fn>( libraryHandle, "hostfxr_initialize_for_runtime_config" );
			s_CoreCLRFunctions.GetRuntimeDelegate = LoadFunctionPtr<hostfxr_get_runtime_delegate_fn>( libraryHandle, "hostfxr_get_runtime_delegate" );
			s_CoreCLRFunctions.CloseHostFXR = LoadFunctionPtr<hostfxr_close_fn>( libraryHandle, "hostfxr_close" );
		}

		bool HostInstance::InitializeSapManaged()
		{
			// Fetch load_assembly_and_get_function_pointer_fn from CoreCLR
			{
				auto runtimeConfigPath = std::filesystem::path( m_Settings.SapDirectory ) / "Tree.Trunk.runtimeconfig.json";

				if ( !std::filesystem::exists( runtimeConfigPath ) )
				{
					MessageCallback( "Failed to find Tree.Trunk.runtimeconfig.json", MessageLevel::Error );
					return false;
				}

				int status = s_CoreCLRFunctions.InitHostFXRForRuntimeConfig( runtimeConfigPath.c_str(), nullptr, &m_HostFXRContext );
				SAP_VERIFY( status == StatusCode::Success || status == StatusCode::Success_HostAlreadyInitialized || status == StatusCode::Success_DifferentRuntimeProperties );
				SAP_VERIFY( m_HostFXRContext != nullptr );

				status = s_CoreCLRFunctions.GetRuntimeDelegate( m_HostFXRContext, hdt_load_assembly_and_get_function_pointer, (void**)&s_CoreCLRFunctions.GetManagedFunctionPtr );
				SAP_VERIFY( status == StatusCode::Success );
			}

			using InitializeFn = void( * )( void( * )( SapString, MessageLevel ), void( * )( SapString ) );
			InitializeFn sapManagedEntryPoint = nullptr;
			sapManagedEntryPoint = LoadSapManagedFunctionPtr<InitializeFn>( SAP_STR( "Tree.Sap.ManagedHost, Tree.Trunk" ), SAP_STR( "Initialize" ) );

			LoadSapFunctions();

			sapManagedEntryPoint( []( SapString InMessage, MessageLevel InLevel )
				{
					if ( MessageFilter & InLevel )
					{
						std::string message = InMessage;
						MessageCallback( message, InLevel );
					}
				},
				[]( SapString InMessage )
				{
					std::string message = InMessage;
					if ( !ExceptionCallback )
					{
						MessageCallback( message, MessageLevel::Error );
						return;
					}

					ExceptionCallback( message );
				} );

			ExceptionCallback = m_Settings.ExceptionCallback;

			return true;
		}

		void HostInstance::LoadSapFunctions()
		{
			s_ManagedFunctions.CreateAssemblyLoadContextFptr = LoadSapManagedFunctionPtr<CreateAssemblyLoadContextFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Trunk" ), SAP_STR( "CreateAssemblyLoadContext" ) );
			s_ManagedFunctions.UnloadAssemblyLoadContextFptr = LoadSapManagedFunctionPtr<UnloadAssemblyLoadContextFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Trunk" ), SAP_STR( "UnloadAssemblyLoadContext" ) );
			s_ManagedFunctions.LoadManagedAssemblyFptr = LoadSapManagedFunctionPtr<LoadManagedAssemblyFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Trunk" ), SAP_STR( "LoadAssembly" ) );
			s_ManagedFunctions.UnloadAssemblyLoadContextFptr = LoadSapManagedFunctionPtr<UnloadAssemblyLoadContextFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Trunk" ), SAP_STR( "UnloadAssemblyLoadContext" ) );
			s_ManagedFunctions.GetLastLoadStatusFptr = LoadSapManagedFunctionPtr<GetLastLoadStatusFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Trunk" ), SAP_STR( "GetLastLoadStatus" ) );
			s_ManagedFunctions.GetAssemblyNameFptr = LoadSapManagedFunctionPtr<GetAssemblyNameFn>( SAP_STR( "Tree.Sap.AssemblyLoader, Tree.Trunk" ), SAP_STR( "GetAssemblyName" ) );

			s_ManagedFunctions.GetAssemblyTypesFptr = LoadSapManagedFunctionPtr<GetAssemblyTypesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetAssemblyTypes" ) );
			s_ManagedFunctions.GetTypeIdFptr = LoadSapManagedFunctionPtr<GetTypeIdFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetTypeId" ) );
			s_ManagedFunctions.GetFullTypeNameFptr = LoadSapManagedFunctionPtr<GetFullTypeNameFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetFullTypeName" ) );
			s_ManagedFunctions.GetAssemblyQualifiedNameFptr = LoadSapManagedFunctionPtr<GetAssemblyQualifiedNameFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetAssemblyQualifiedName" ) );
			s_ManagedFunctions.GetBaseTypeFptr = LoadSapManagedFunctionPtr<GetBaseTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetBaseType" ) );
			s_ManagedFunctions.GetTypeSizeFptr = LoadSapManagedFunctionPtr<GetTypeSizeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetTypeSize" ) );
			s_ManagedFunctions.IsTypeSubclassOfFptr = LoadSapManagedFunctionPtr<IsTypeSubclassOfFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "IsTypeSubclassOf" ) );
			s_ManagedFunctions.IsTypeAssignableToFptr = LoadSapManagedFunctionPtr<IsTypeAssignableToFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "IsTypeAssignableTo" ) );
			s_ManagedFunctions.IsTypeAssignableFromFptr = LoadSapManagedFunctionPtr<IsTypeAssignableFromFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "IsTypeAssignableFrom" ) );
			s_ManagedFunctions.IsTypeSZArrayFptr = LoadSapManagedFunctionPtr<IsTypeSZArrayFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "IsTypeSZArray" ) );
			s_ManagedFunctions.GetElementTypeFptr = LoadSapManagedFunctionPtr<GetElementTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetElementType" ) );
			s_ManagedFunctions.GetTypeMethodsFptr = LoadSapManagedFunctionPtr<GetTypeMethodsFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetTypeMethods" ) );
			s_ManagedFunctions.GetTypeFieldsFptr = LoadSapManagedFunctionPtr<GetTypeFieldsFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetTypeFields" ) );
			s_ManagedFunctions.GetTypePropertiesFptr = LoadSapManagedFunctionPtr<GetTypePropertiesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetTypeProperties" ) );
			s_ManagedFunctions.HasTypeAttributeFptr = LoadSapManagedFunctionPtr<HasTypeAttributeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "HasTypeAttribute" ) );
			s_ManagedFunctions.GetTypeAttributesFptr = LoadSapManagedFunctionPtr<GetTypeAttributesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetTypeAttributes" ) );
			s_ManagedFunctions.GetTypeManagedTypeFptr = LoadSapManagedFunctionPtr<GetTypeManagedTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetTypeManagedType" ) );
			s_ManagedFunctions.InvokeStaticMethodFptr = LoadSapManagedFunctionPtr<InvokeStaticMethodFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Trunk" ), SAP_STR( "InvokeStaticMethod" ) );
			s_ManagedFunctions.InvokeStaticMethodRetFptr = LoadSapManagedFunctionPtr<InvokeStaticMethodRetFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Trunk" ), SAP_STR( "InvokeStaticMethodRet" ) );

			s_ManagedFunctions.GetMethodInfoNameFptr = LoadSapManagedFunctionPtr<GetMethodInfoNameFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetMethodInfoName" ) );
			s_ManagedFunctions.GetMethodInfoReturnTypeFptr = LoadSapManagedFunctionPtr<GetMethodInfoReturnTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetMethodInfoReturnType" ) );
			s_ManagedFunctions.GetMethodInfoParameterTypesFptr = LoadSapManagedFunctionPtr<GetMethodInfoParameterTypesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetMethodInfoParameterTypes" ) );
			s_ManagedFunctions.GetMethodInfoAccessibilityFptr = LoadSapManagedFunctionPtr<GetMethodInfoAccessibilityFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetMethodInfoAccessibility" ) );
			s_ManagedFunctions.GetMethodInfoAttributesFptr = LoadSapManagedFunctionPtr<GetMethodInfoAttributesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetMethodInfoAttributes" ) );

			s_ManagedFunctions.GetFieldInfoNameFptr = LoadSapManagedFunctionPtr<GetFieldInfoNameFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetFieldInfoName" ) );
			s_ManagedFunctions.GetFieldInfoTypeFptr = LoadSapManagedFunctionPtr<GetFieldInfoTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetFieldInfoType" ) );
			s_ManagedFunctions.GetFieldInfoAccessibilityFptr = LoadSapManagedFunctionPtr<GetFieldInfoAccessibilityFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetFieldInfoAccessibility" ) );
			s_ManagedFunctions.GetFieldInfoAttributesFptr = LoadSapManagedFunctionPtr<GetFieldInfoAttributesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetFieldInfoAttributes" ) );

			s_ManagedFunctions.GetPropertyInfoNameFptr = LoadSapManagedFunctionPtr<GetPropertyInfoNameFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetPropertyInfoName" ) );
			s_ManagedFunctions.GetPropertyInfoTypeFptr = LoadSapManagedFunctionPtr<GetPropertyInfoTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetPropertyInfoType" ) );
			s_ManagedFunctions.GetPropertyInfoAttributesFptr = LoadSapManagedFunctionPtr<GetPropertyInfoAttributesFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetPropertyInfoAttributes" ) );

			s_ManagedFunctions.GetAttributeFieldValueFptr = LoadSapManagedFunctionPtr<GetAttributeFieldValueFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetAttributeFieldValue" ) );
			s_ManagedFunctions.GetAttributeTypeFptr = LoadSapManagedFunctionPtr<GetAttributeTypeFn>( SAP_STR( "Tree.Sap.TypeInterface, Tree.Trunk" ), SAP_STR( "GetAttributeType" ) );

			s_ManagedFunctions.SetInternalCallsFptr = LoadSapManagedFunctionPtr<SetInternalCallsFn>( SAP_STR( "Tree.Sap.Interop.InternalCallsManager, Tree.Trunk" ), SAP_STR( "SetInternalCalls" ) );
			s_ManagedFunctions.CreateObjectFptr = LoadSapManagedFunctionPtr<CreateObjectFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Trunk" ), SAP_STR( "CreateObject" ) );
			s_ManagedFunctions.InvokeMethodFptr = LoadSapManagedFunctionPtr<InvokeMethodFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Trunk" ), SAP_STR( "InvokeMethod" ) );
			s_ManagedFunctions.InvokeMethodRetFptr = LoadSapManagedFunctionPtr<InvokeMethodRetFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Trunk" ), SAP_STR( "InvokeMethodRet" ) );
			s_ManagedFunctions.SetFieldValueFptr = LoadSapManagedFunctionPtr<SetFieldValueFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Trunk" ), SAP_STR( "SetFieldValue" ) );
			s_ManagedFunctions.GetFieldValueFptr = LoadSapManagedFunctionPtr<GetFieldValueFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Trunk" ), SAP_STR( "GetFieldValue" ) );
			s_ManagedFunctions.SetPropertyValueFptr = LoadSapManagedFunctionPtr<SetFieldValueFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Trunk" ), SAP_STR( "SetPropertyValue" ) );
			s_ManagedFunctions.GetPropertyValueFptr = LoadSapManagedFunctionPtr<GetFieldValueFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Trunk" ), SAP_STR( "GetPropertyValue" ) );
			s_ManagedFunctions.DestroyObjectFptr = LoadSapManagedFunctionPtr<DestroyObjectFn>( SAP_STR( "Tree.Sap.ManagedObject, Tree.Trunk" ), SAP_STR( "DestroyObject" ) );
			s_ManagedFunctions.CollectGarbageFptr = LoadSapManagedFunctionPtr<CollectGarbageFn>( SAP_STR( "Tree.Sap.GarbageCollector, Tree.Trunk" ), SAP_STR( "CollectGarbage" ) );
			s_ManagedFunctions.WaitForPendingFinalizersFptr = LoadSapManagedFunctionPtr<WaitForPendingFinalizersFn>( SAP_STR( "Tree.Sap.GarbageCollector, Tree.Trunk" ), SAP_STR( "WaitForPendingFinalizers" ) );
		}

		void* HostInstance::LoadSapManagedFunctionPtr( const std::filesystem::path& InAssemblyPath, const SapChar* InTypeName, const SapChar* InMethodName, const SapChar* InDelegateType ) const
		{
			void* funcPtr = nullptr;
			int status = s_CoreCLRFunctions.GetManagedFunctionPtr( InAssemblyPath.c_str(), InTypeName, InMethodName, InDelegateType, nullptr, &funcPtr );
			SAP_VERIFY( status == StatusCode::Success && funcPtr != nullptr );
			return funcPtr;
		}
	}
}
