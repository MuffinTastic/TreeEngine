#pragma once

#include <string>

namespace Tree::Platform
{
	// These return absolute (fully qualified) paths
	std::string GetExecutableFile();
	std::string GetExecutableDirectory();

	struct SharedLibrary;
	SharedLibrary* LoadSharedLibrary( std::string path );
	void* GetSharedLibraryFunc( SharedLibrary* sharedLibrary, std::string name );

	int ChangeCurrentDirectory( std::string path );
}

#define EXPORT extern "C" __declspec( dllexport )