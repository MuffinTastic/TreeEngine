#include "string.h"

#include <algorithm>

char* Tree::CopyToCString( const std::string_view text )
{
	size_t len = text.size() + 1; // Add 1 for null terminator
	char* str = new char[len];
	std::copy( text.data(), text.data() + len, str );
	return str;
}

void Tree::DeleteCString( char* str )
{
	delete[] str;
}
