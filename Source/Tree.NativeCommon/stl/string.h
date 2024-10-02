#pragma once

#include <string>

#include "Tree.NativeCommon/memory.h"


template <typename CharT, typename Traits = std::char_traits<CharT>>
using TreeBasicString = std::basic_string<CharT, Traits, TreeHeapAlloc<CharT>>;
using TreeString = TreeBasicString<char>;
using TreeWString = TreeBasicString<wchar_t>;