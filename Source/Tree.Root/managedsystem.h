#pragma once

#include "interfaces/imanagedsystem.h"



namespace Tree
{
	class ManagedSystem : public IManagedSystem
	{
	public:
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

	private:
	};
}