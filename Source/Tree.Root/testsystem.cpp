#include "testsystem.h"

#include <iostream>

#include "Tree.NativeCommon/sys.h"

REGISTER_TREE_SYSTEM( TestSystem, TESTSYSTEM_NAME )

Tree::ESystemInitCode Tree::TestSystem::Startup()
{
    std::cout << "Test system startup" << std::endl;

    return ESYSTEMINIT_SUCCESS;
}

void Tree::TestSystem::Shutdown()
{
    std::cout << "Test system shutdown" << std::endl;
}

void Tree::TestSystem::RootSpecific()
{
    std::cout << "Test system root specific code" << std::endl;
    Sys::Engine()->Shutdown();
}
