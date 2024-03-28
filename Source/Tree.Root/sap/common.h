// Modified from https://github.com/StudioCherno/Coral

// MIT License
// 
// Copyright( c ) 2023 Studio Cherno
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstddef>
#include <string_view>

#ifdef WINDOWS
	#define SAP_CALLTYPE __cdecl

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
	
	using SapChar = char;
	using SapStringView = std::string_view;
#endif

using SapBool32 = uint32_t;