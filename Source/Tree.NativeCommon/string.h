#pragma once

#include <string_view>

namespace Tree
{
	// You MUST deallocate this yourself!
	char* CopyToCString( const std::string_view text );
	void DeleteCString( char* str );
}