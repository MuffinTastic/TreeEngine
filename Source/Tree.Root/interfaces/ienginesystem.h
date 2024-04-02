#pragma once

#include "Tree.NativeCommon/system.h"
#include "Tree.NativeCommon/domain.h"

#define ENGINESYSTEM_NAME "EngineSystem_v01"

namespace Tree
{
	struct EngineStartupConfig
	{
		EDomain domain;
	};

	class IEngineSystem : public ISystem
	{
	public:
		virtual void SetStartupConfig( EngineStartupConfig config ) = 0;
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;

		virtual void Run() = 0;

		SAP_GEN virtual inline EDomain GetDomain() const = 0;
		virtual inline bool IsClient() const = 0;
		virtual inline bool IsClientOnly() const = 0;
		virtual inline bool IsEditorOnly() const = 0;
		virtual inline bool IsDedicatedServer() const = 0;
	};
}