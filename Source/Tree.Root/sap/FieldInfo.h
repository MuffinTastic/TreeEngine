﻿#pragma once

#include <vector>

#include "Core.h"
#include "String.h"

namespace Tree
{
	namespace Sap
	{
		class Type;
		class Attribute;

		class FieldInfo
		{
		public:
			String GetName() const;
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
