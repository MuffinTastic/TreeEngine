#pragma once

#include <string>

#include "Tree.NativeCommon/defs.h"

namespace Tree
{
	struct SAP_GEN WindowInfo
	{

	};

	class IWindow
	{
	public:
		SAP_GEN virtual std::string GetInternalName() const = 0;
	};
}