#pragma once

namespace Tree
{
	enum EDomain
	{
		EDOMAIN_CLIENT				= 0b001,
		EDOMAIN_EDITOR				= 0b011,
		EDOMAIN_DEDICATED_SERVER	= 0b100,

		EDOMAIN_EDITOR_ONLY			= 0b010
	};

	constexpr bool DomainIsClient( EDomain domain )
	{
		return ( domain & EDOMAIN_CLIENT ) == EDOMAIN_CLIENT;
	}

	constexpr bool DomainIsClientOnly( EDomain domain )
	{
		return domain == EDOMAIN_CLIENT;
	}

	constexpr bool DomainIsEditorOnly( EDomain domain )
	{
		return ( domain & EDOMAIN_EDITOR_ONLY ) == EDOMAIN_EDITOR_ONLY;
	}

	constexpr bool DomainIsDedicatedServer( EDomain domain )
	{
		return domain == EDOMAIN_DEDICATED_SERVER;
	}
}

#ifndef DEDICATED_SERVER
#define GUI_ENABLED
#endif
