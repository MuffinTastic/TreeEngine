#pragma once

#include <vector>

#include "Core.h"
#include "String.h"

namespace Tree
{
	namespace Sap
	{
		class Type;
		class Attribute;

		class PropertyInfo
		{
		public:
			String GetName() const;
			Type& GetType();

			std::vector<Attribute> GetAttributes() const;

		private:
			ManagedHandle m_Handle = -1;
			Type* m_Type = nullptr;

			friend class Type;
		};
	}
}
