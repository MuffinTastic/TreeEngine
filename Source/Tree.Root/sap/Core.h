#pragma once

#include <string_view>

#ifdef WINDOWS
	#define SAP_CALLTYPE __cdecl
	#define SAP_HOSTFXR_NAME "hostfxr.dll"

	#ifdef _WCHAR_T_DEFINED
		#define SAP_STR(s) L##s
		#define SAP_WIDE_CHARS

		using SapChar = wchar_t;
		using SapStringView = std::wstring_view;

	#else
		#define SAP_STR(s) s

		using SapChar = unsigned short;
		using SapStringView = std::string_view;
	#endif
#else
	#define SAP_CALLTYPE
	#define SAP_STR(s) s
	#define SAP_HOSTFXR_NAME "libhostfxr.so"

	using SapChar = char;
	using SapStringView = std::string_view;
#endif

#define SAP_DOTNET_TARGET_VERSION_MAJOR 8
#define SAP_DOTNET_TARGET_VERSION_MAJOR_STR '8'
#define SAP_UNMANAGED_CALLERS_ONLY ((const SapChar*)-1)

namespace Tree
{
	namespace Sap
	{
		using Bool32 = uint32_t;

		enum class TypeAccessibility
		{
			Public,
			Private,
			Protected,
			Internal,
			ProtectedPublic,
			PrivateProtected
		};

		using TypeId = int32_t;
		using ManagedHandle = int32_t;

		struct InternalCall
		{
			const SapChar* Name;
			void* NativeFunctionPtr;
		};

	}
}
