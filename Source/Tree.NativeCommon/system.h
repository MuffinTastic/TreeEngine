#pragma once

#include <string>
#include <unordered_map>

#include "platform.h"

namespace Tree
{
	enum ESystemInitCode
	{
		ESYSTEMINIT_SUCCESS,
		ESYSTEMINIT_FAILURE
	};

	class System
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;
	};

	class SystemRegistry
	{
	public:
		SystemRegistry( System* system, std::string_view name );

	public:
		System* m_System;
		std::string m_Name;

	public:
		static std::unordered_map<std::string, System*>& GetSystems();
		static void Register( SystemRegistry& registry );
		static System* GetSystem( std::string name );
	};
}

// Register the system with the current Module (see module.h/cpp)
#define REGISTER_TREE_SYSTEM( classname, registryname ) \
	static Tree::classname __gts_##classname;	/* System instance! */ \
	static Tree::SystemRegistry __gtsr_##classname( &__gts_##classname, registryname );