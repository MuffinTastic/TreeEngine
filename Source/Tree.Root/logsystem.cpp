#include "interfaces/ilogsystem.h"

#include "Tree.NativeCommon/sys.h"

namespace Tree
{
	class LogSystem : public ILogSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;
	};
}


REGISTER_TREE_SYSTEM( LogSystem, LOGSYSTEM_NAME )


Tree::ESystemInitCode Tree::LogSystem::Startup()
{


    return ESYSTEMINIT_SUCCESS;
}

void Tree::LogSystem::Shutdown()
{

}