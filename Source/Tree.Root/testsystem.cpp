#include "testsystem.h"

#include "Tree.NativeCommon/sys.h"


REGISTER_TREE_SYSTEM( TestSystem, TESTSYSTEM_NAME )


Tree::ESystemInitCode Tree::TestSystem::Startup()
{
    return ESYSTEMINIT_SUCCESS;
}

void Tree::TestSystem::Shutdown()
{

}