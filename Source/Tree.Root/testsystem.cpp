#include "interfaces/itestsystem.h"

#include "Tree.NativeCommon/sys.h"

namespace Tree
{
	class TestSystem : public ITestSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;
	};
}


REGISTER_TREE_SYSTEM( TestSystem, TESTSYSTEM_NAME )


Tree::ESystemInitCode Tree::TestSystem::Startup()
{


    return ESYSTEMINIT_SUCCESS;
}

void Tree::TestSystem::Shutdown()
{

}