#pragma once

#include "Tree.NativeCommon/system.h"

#include <vector>
#include <string>

#include "Tree.Root/sap/Array.h"
#include "Tree.Root/sap/String.h"

#define CMDLINESYSTEM_NAME "CmdLineSystem_v01"

namespace Tree
{
	class ICmdLineSystem : public ISystem
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;

		virtual void ProcessArguments( std::vector<std::string> arguments ) = 0;

		virtual std::vector<std::string> GetArguments() const = 0;

		SAP_GEN virtual bool GetFlag( std::string name ) const = 0;
		SAP_GEN virtual std::string GetStringOption( std::string name, std::string def ) const = 0;
		SAP_GEN virtual int GetIntOption( std::string name, int def ) const = 0;

		virtual std::vector<std::string> GetCommands() const = 0;
	};
}