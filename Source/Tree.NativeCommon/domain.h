#pragma once

#include "defs.h"

namespace Tree
{
	enum SAP_GEN EDomain
	{
		EDOMAIN_UNINITIALIZED		= 0,

		EDOMAIN_EDITOR				= 0b011,
		EDOMAIN_GAME				= 0b001,
		EDOMAIN_DEDICATED_SERVER	= 0b100,

		EDOMAIN_EDITOR_ONLY			= 0b010
	};
}