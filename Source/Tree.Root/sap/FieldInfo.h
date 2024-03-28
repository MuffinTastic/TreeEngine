#pragma once

#include <vector>

#include "Core.h"
#include "SapString.h"

namespace Tree
{
	namespace Sap
	{
		class Type;
		class Attribute;

		class FieldInfo
		{
		public:
			SapString GetName() const;
			Type& GetType();

			TypeAccessibility GetAccessibility() const;

			std::vector<Attribute> GetAttributes() const;

		private:
			ManagedHandle m_Handle = -1;
			Type* m_Type = nullptr;

			friend class Type;
		};
	}
}
