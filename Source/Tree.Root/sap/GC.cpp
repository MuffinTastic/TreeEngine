﻿#include "GC.h"
#include "SapManagedFunctions.h"

namespace Tree
{
	namespace Sap
	{
		void GC::Collect()
		{
			Collect( -1, GCCollectionMode::Default, true, false );
		}

		void GC::Collect( int32_t InGeneration, GCCollectionMode InCollectionMode, bool InBlocking, bool InCompacting )
		{
			s_ManagedFunctions.CollectGarbageFptr( InGeneration, InCollectionMode, InBlocking, InCompacting );
		}

		void GC::WaitForPendingFinalizers()
		{
			s_ManagedFunctions.WaitForPendingFinalizersFptr();
		}
	}
}
