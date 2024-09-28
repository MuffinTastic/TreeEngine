#pragma once

#include <string>
#include <unordered_map>

#include "defs.h"
#include "platform.h"

namespace Tree
{
	enum ESystemInitCode
	{
		ESYSTEMINIT_SUCCESS,
		ESYSTEMINIT_FAILURE
	};

	class ISystem
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;
	};

	class SystemRegistry
	{
	public:
		SystemRegistry( ISystem* system, std::string_view name );

	public:
		ISystem* m_system;
		std::string m_name;

	public:
		static std::unordered_map<std::string, ISystem*>& GetSystems();
		static void Register( SystemRegistry& registry );
		static ISystem* GetSystem( std::string name );
	};
}

// Register the system with the local SysGroup (see module.h/cpp)
#define REGISTER_TREE_SYSTEM( classname, registryname ) \
	static Tree::classname __gts_##classname;	/* ISystem instance! */ \
	static Tree::SystemRegistry __gtsr_##classname( &__gts_##classname, registryname );