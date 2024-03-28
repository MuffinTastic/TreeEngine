#pragma once

#include <string>

#define UTF_CPP_CPLUSPLUS 202002L
#include "utfcpp/utf8.h"

// This makes up for some oversights in utf8
namespace utf8
{
#ifdef WINDOWS
    inline std::string wcharto8( std::wstring_view s )
    {
        std::string result;
        utf16to8( s.begin(), s.end(), std::back_inserter( result ) );
        return result;
    }

    inline std::wstring utf8towchar( std::string_view s )
    {
        std::wstring result;
        utf8to16( s.begin(), s.end(), std::back_inserter( result ) );
        return result;
    }
#endif
}