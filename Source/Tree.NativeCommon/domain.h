#pragma once

namespace Tree
{
	enum EDomain
	{
		EDOMAIN_EDITOR				= 0b011,
		EDOMAIN_CLIENT				= 0b001,
		EDOMAIN_DEDICATED_SERVER	= 0b100,

		EDOMAIN_EDITOR_ONLY			= 0b010
	};
}