#include "interfaces/imanagedsystem.h"

#include "Tree.NativeCommon/sys.h"

namespace Tree
{
	class ManagedSystem : public IManagedSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

	private:
	};
}


REGISTER_TREE_SYSTEM( ManagedSystem, MANAGEDSYSTEM_NAME )


Tree::ESystemInitCode Tree::ManagedSystem::Startup()
{


    return ESYSTEMINIT_SUCCESS;
}

void Tree::ManagedSystem::Shutdown()
{

}