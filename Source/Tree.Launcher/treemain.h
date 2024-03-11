#pragma once

#include <string>
#include <vector>

#define TREEMAIN_SUCCESS			0
#define TREEMAIN_FAILURE_MODULE		1
#define TREEMAIN_FAILURE_SYSTEM		2

namespace Tree
{
	int TreeMain( std::vector<std::string> arguments );
}