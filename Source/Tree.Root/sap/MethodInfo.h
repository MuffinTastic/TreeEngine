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

		class MethodInfo
		{
		public:
			String GetName() const;

			Type& GetReturnType();
			const std::vector<Type*>& GetParameterTypes();

			TypeAccessibility GetAccessibility() const;

			std::vector<Attribute> GetAttributes() const;

		private:
			ManagedHandle m_Handle = -1;
			Type* m_ReturnType = nullptr;
			std::vector<Type*> m_ParameterTypes;

			friend class Type;
		};
	}
}