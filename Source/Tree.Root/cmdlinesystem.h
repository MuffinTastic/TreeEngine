#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>

#include "interfaces/icmdlinesystem.h"

namespace Tree
{
	class CmdLineSystem : public ICmdLineSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		virtual void ProcessArguments( std::vector<std::string> arguments ) override;

		virtual std::vector<std::string> GetArguments() const override;

		virtual bool GetFlag( std::string name ) const override;
		virtual std::string GetStringOption( std::string name, std::string def ) const override;
		virtual int GetIntOption( std::string name, int def ) const override;

		virtual std::vector<std::string> GetCommands() const override;

	private:
		void InsertCommand( std::string_view first, std::optional<std::string_view> second );

		std::vector<std::string> m_arguments;

		std::unordered_set<std::string> m_flags;
		std::unordered_map<std::string, std::string> m_options;
		std::vector<std::string> m_commands;
	};
}