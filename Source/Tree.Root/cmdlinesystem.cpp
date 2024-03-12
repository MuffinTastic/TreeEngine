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
		virtual void SetArguments( std::vector<std::string> arguments ) override;
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		virtual std::vector<std::string> GetArguments() const override;

		virtual bool HasItem( std::string ) const override;
		virtual bool GetFlag( std::string name ) const override;
		virtual std::string GetString( std::string name, std::string def ) const override;
		virtual int GetInt( std::string name, int def ) const override;

	private:
		void ProcessArguments();

		std::vector<std::string> m_arguments;

		std::vector<std::string> m_flags;
		std::vector<std::tuple<std::string, std::string>> m_pairs;
	};
}


REGISTER_TREE_SYSTEM( CmdLineSystem, CMDLINESYSTEM_NAME )


void Tree::CmdLineSystem::SetArguments( std::vector<std::string> arguments )
{
    m_arguments = arguments;
}

Tree::ESystemInitCode Tree::CmdLineSystem::Startup()
{
    ProcessArguments();

    return ESYSTEMINIT_SUCCESS;
}

void Tree::CmdLineSystem::Shutdown()
{

}

std::vector<std::string> Tree::CmdLineSystem::GetArguments() const
{
    return m_arguments;
}

void Tree::CmdLineSystem::ProcessArguments()
{
    
}
