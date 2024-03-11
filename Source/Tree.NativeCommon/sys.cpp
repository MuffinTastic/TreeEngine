#include "sys.h"

#include "module.h"

namespace Tree::Sys
{
	static IEngineSystem* s_engine = nullptr;
	IEngineSystem* Engine() { return s_engine; }

	void UpdateFromModule( Module* module )
	{
		if ( !s_engine )
			s_engine = dynamic_cast<IEngineSystem*>( module->GetSystem( ENGINESYSTEM_NAME ) );
	}

	void Reset()
	{
		s_engine = nullptr;
	}
}