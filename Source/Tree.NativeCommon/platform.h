#pragma once

#include <string>

#include <functional>

namespace Tree::Platform
{
	// These return absolute (fully qualified) paths
	std::string GetExecutableFile();
	std::string GetExecutableDirectory();

	struct SharedLibrary;
	SharedLibrary* LoadSharedLibrary( std::string path );

	void* GetSharedLibraryFunc( SharedLibrary* sharedLibrary, std::string name );

	template <typename T>
	std::function<T> GetSharedLibraryFunc( SharedLibrary* sharedLibrary, std::string name )
	{
		void* ptr = Platform::GetSharedLibraryFunc( sharedLibrary, name );
		std::function<T> func( reinterpret_cast<T*>( ptr ) );
		return func;
	}

	int ChangeCurrentDirectory( std::string path );
}

#define EXPORT extern "C" __declspec( dllexport )