#pragma once

#include <corehost/hostfxr.h>
#include <corehost/coreclr_delegates.h>

#include "sap/Core.h"
#include "sap/Assembly.h"

#include "interfaces/imanagedhostsystem.h"
#include "interfaces/ilogsystem.h"

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

		virtual Sap::Bool32 TrunkStartup() override;
		virtual void TrunkShutdown() override;

		virtual void TrunkUpdateHook() override;
		virtual void TrunkPreSceneRenderHook() override;
		virtual void TrunkPostScenePreUIRenderHook() override;
		virtual void TrunkPostUIRenderHook() override;

	public:
		std::shared_ptr<ILogger> m_logger;

	private:
		Sap::AssemblyLoadContext m_loadContext;
		Sap::Type m_trunkEntry;

		void LoadEngineFunctions();

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

		static void HostFXRErrorCallback( const SapChar* InMessage );
		static void ExceptionCallback( Sap::String InMessage );
		static void InfoCallback( Sap::String InMessage );
		static void WarningCallback( Sap::String InMessage );
		static void ErrorCallback( Sap::String InMessage );

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