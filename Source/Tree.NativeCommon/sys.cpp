#include "sys.h"

#include "module.h"

namespace Tree::Sys
{
	static IEngineSystem* s_Engine = nullptr;
	IEngineSystem* Engine() { return s_Engine; }

	void UpdateFromModule( Module* module )
	{
		if ( !s_Engine )
			s_Engine = dynamic_cast<IEngineSystem*>( module->GetSystem( ENGINESYSTEM_NAME ) );
	}

	void Reset()
	{
		s_Engine = nullptr;
	}
}