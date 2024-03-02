#pragma once

#include <string>

namespace Tree::Platform
{
	// These return absolute (fully qualified) paths
	std::string GetExecutableFile();
	std::string GetExecutableDirectory();

	struct Library;
	Library* LoadSharedLibrary( std::string path );

	void ChangeDirectory( std::string path );
}