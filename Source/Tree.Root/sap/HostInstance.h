#pragma once

#include "Core.h"
#include "MessageLevel.h"
#include "Assembly.h"
#include "ManagedObject.h"

#include <functional>
#include <filesystem>

namespace Tree
{
	namespace Sap
	{
		using ExceptionCallbackFn = std::function<void( std::string_view )>;

		struct HostSettings
		{
			/// <summary>
			/// The file path to Coral.runtimeconfig.json (e.g C:\Dev\MyProject\ThirdParty\Coral)
			/// </summary>
			std::string CoralDirectory;

			MessageCallbackFn MessageCallback = nullptr;
			MessageLevel MessageFilter = MessageLevel::All;

			ExceptionCallbackFn ExceptionCallback = nullptr;
		};

		class HostInstance
		{
		public:
			bool Initialize( HostSettings InSettings );
			void Shutdown();

			AssemblyLoadContext CreateAssemblyLoadContext( std::string_view InName );
			void UnloadAssemblyLoadContext( AssemblyLoadContext& InLoadContext );

		private:
			void LoadHostFXR() const;
			bool InitializeCoralManaged();
			void LoadCoralFunctions();

			void* LoadSapManagedFunctionPtr( const std::filesystem::path& InAssemblyPath, const SapChar* InTypeName, const SapChar* InMethodName, const SapChar* InDelegateType = SAP_UNMANAGED_CALLERS_ONLY ) const;

			template<typename TFunc>
			TFunc LoadSapManagedFunctionPtr( const SapChar* InTypeName, const SapChar* InMethodName, const SapChar* InDelegateType = SAP_UNMANAGED_CALLERS_ONLY ) const
			{
				return (TFunc)LoadSapManagedFunctionPtr( m_CoralManagedAssemblyPath, InTypeName, InMethodName, InDelegateType );
			}

		private:
			HostSettings m_Settings;
			std::filesystem::path m_CoralManagedAssemblyPath;
			void* m_HostFXRContext = nullptr;
			bool m_Initialized = false;

			friend class AssemblyLoadContext;
		};
	}
}
