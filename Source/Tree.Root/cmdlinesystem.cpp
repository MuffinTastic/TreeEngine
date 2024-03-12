#include "interfaces/icmdlinesystem.h"

#include <vector>
#include <string>
#include <tuple>

#include "Tree.NativeCommon/sys.h"

namespace Tree
{
	class CmdLineSystem : public ICmdLineSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		virtual void ProcessArguments( std::vector<std::string> arguments ) override;
		virtual std::vector<std::string> GetArguments() const override;

		virtual bool HasItem( std::string ) const override;
		virtual bool GetFlag( std::string name ) const override;
		virtual std::string GetString( std::string name, std::string def ) const override;
		virtual int GetInt( std::string name, int def ) const override;

	private:

		std::vector<std::string> m_arguments;

		std::vector<std::string> m_flags;
		std::vector<std::tuple<std::string, std::string>> m_pairs;
	};
}


REGISTER_TREE_SYSTEM( CmdLineSystem, CMDLINESYSTEM_NAME )


Tree::ESystemInitCode Tree::CmdLineSystem::Startup()
{
    return ESYSTEMINIT_SUCCESS;
}

void Tree::CmdLineSystem::Shutdown()
{

}

void Tree::CmdLineSystem::ProcessArguments( std::vector<std::string> arguments )
{
	m_arguments = arguments;
}

std::vector<std::string> Tree::CmdLineSystem::GetArguments() const
{
	return m_arguments;
}