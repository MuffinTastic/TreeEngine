#pragma once

#include "Tree.NativeCommon/debug.h"
#include "Tree.NativeCommon/platform.h"


// TODO: Log error using Platform::DebugLog
#define SAP_VERIFY(expr) {\
						if(!(expr))\
						{\
							Tree::Platform::DebugLog("Managed host verification failed");\
							TREE_DEBUG_BREAK;\
						}\
					}
