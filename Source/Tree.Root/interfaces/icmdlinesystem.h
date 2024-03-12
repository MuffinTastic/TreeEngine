#pragma once

#include "Tree.NativeCommon/system.h"

#include <vector>
#include <string>

#define CMDLINESYSTEM_NAME "CmdLineSystem_v01"

namespace Tree
{
	class ICmdLineSystem : public System
	{
	public:
		virtual void SetArguments( std::vector<std::string> arguments ) = 0;
		
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;

		virtual std::vector<std::string> GetArguments() const = 0;

		virtual bool HasItem( std::string ) const = 0;
		virtual bool GetFlag( std::string name ) const = 0;
		virtual std::string GetString( std::string name, std::string def ) const = 0;
		virtual int GetInt( std::string name, int def ) const = 0;
	};
}