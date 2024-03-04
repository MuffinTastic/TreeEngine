#include "enginesystem.h"

#include <iostream>

REGISTER_TREE_SYSTEM( EngineSystem, ENGINESYSTEM_NAME )

Tree::ESystemInitCode Tree::EngineSystem::Startup()
{


    return ESYSTEMINIT_SUCCESS;
}

void Tree::EngineSystem::Shutdown()
{
    std::cout << "Engine shutdown" << std::endl;
}
