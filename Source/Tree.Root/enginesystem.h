#pragma once

#include "Tree.NativeCommon/domain.h"

#include "interfaces/ienginesystem.h"
#include "interfaces/ilogsystem.h"

namespace Tree
{
	class EngineSystem : public IEngineSystem
	{
	public:
		virtual void SetStartupConfig( EngineStartupConfig config ) override;
		virtual ESystemInitCode Startup() override;
		virtual void Shutdown() override;

		void Run();

		virtual inline EDomain GetDomain() const override;
		virtual inline bool IsClient() const override;
		virtual inline bool IsClientOnly() const override;
		virtual inline bool IsEditorOnly() const override;
		virtual inline bool IsDedicatedServer() const override;

	private:
		std::shared_ptr<ILogger> m_logger;
		EDomain m_domain = EDOMAIN_UNINITIALIZED;
	};
}