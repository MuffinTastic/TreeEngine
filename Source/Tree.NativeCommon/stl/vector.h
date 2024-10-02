#pragma once

#include <vector>

#include "Tree.NativeCommon/memory.h"


template <typename T>
using TreeVector = std::vector<T, TreeHeapAlloc<T>>;