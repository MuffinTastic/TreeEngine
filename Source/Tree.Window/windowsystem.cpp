#include "windowsystem.h"

#include "Tree.NativeCommon/sys.h"


REGISTER_TREE_SYSTEM( WindowSystem, WINDOWSYSTEM_NAME )


Tree::ESystemInitCode Tree::WindowSystem::Startup()
{
	m_logger = Sys::Log()->CreateLogger( "Window" );

	m_logger->Info( "Window system startup" );

	return ESYSTEMINIT_SUCCESS;
}

void Tree::WindowSystem::Shutdown()
{

}
