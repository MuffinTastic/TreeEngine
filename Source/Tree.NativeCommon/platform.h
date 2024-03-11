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
	void UnloadSharedLibrary( SharedLibrary* sharedLibrary );

	void* GetSharedLibraryFunc( SharedLibrary* sharedLibrary, std::string name );

	template <typename T>
	std::function<T> GetSharedLibraryFunc( SharedLibrary* sharedLibrary, std::string name )
	{
		void* ptr = Platform::GetSharedLibraryFunc( sharedLibrary, name );
		std::function<T> func( reinterpret_cast<T*>( ptr ) );
		return func;
	}

	int ChangeCurrentDirectory( std::string path );

	void ShowError( std::string text );
	void FatalExit();
}

#define EXPORT extern "C" __declspec( dllexport )

#ifdef WINDOWS
#define SHAREDLIB_EXT ".dll"
#elif LINUX
#define SHAREDLIB_EXT ".so"
#endif