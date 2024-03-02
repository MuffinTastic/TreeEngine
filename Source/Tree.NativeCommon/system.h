#pragma once

#include <string>

namespace Tree
{
	enum ESystemInitCode
	{
		ESYSTEMINIT_SUCCESS,
		ESYSTEMINIT_FAILURE
	};

	class System
	{
	public:
		virtual ESystemInitCode Startup() = 0;
		virtual void Shutdown() = 0;

	private:
		System();

	};


}