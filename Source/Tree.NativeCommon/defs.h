#pragma once

#define ENGINE_NAME "TreeEngine"

#if defined( __clang__ )
#define SAP_GEN __attribute__((annotate("sap_gen")))
#else
#define SAP_GEN
#endif