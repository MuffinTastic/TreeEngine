#include "enginesystem.h"

#include <iostream>

REGISTER_TREE_SYSTEM( EngineSystem, ENGINESYSTEM_NAME )

void Tree::EngineSystem::SetStartupConfig( EngineStartupConfig config )
{

}

Tree::ESystemInitCode Tree::EngineSystem::Startup()
{


    return ESYSTEMINIT_SUCCESS;
}

void Tree::EngineSystem::Shutdown()
{
    std::cout << "Engine shutdown" << std::endl;
}

void Tree::EngineSystem::Run()
{

}
