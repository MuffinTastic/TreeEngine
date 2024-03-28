#include "Attribute.h"
#include "Type.h"
#include "SapManagedFunctions.h"
#include "TypeCache.h"
#include "SapString.h"

namespace Tree
{
	namespace Sap
	{
		Type& Attribute::GetType()
		{
			if ( !m_Type )
			{
				Type type;
				s_ManagedFunctions.GetAttributeTypeFptr( m_Handle, &type.m_Id );
				m_Type = TypeCache::Get().CacheType( std::move( type ) );
			}

			return *m_Type;
		}

		void Attribute::GetFieldValueInternal( std::string_view InFieldName, void* OutValue ) const
		{
			auto fieldName = SapString::New( InFieldName );
			s_ManagedFunctions.GetAttributeFieldValueFptr( m_Handle, fieldName, OutValue );
			SapString::Free( fieldName );
		}

	}
}
