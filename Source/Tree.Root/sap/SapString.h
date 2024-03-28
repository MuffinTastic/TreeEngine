#pragma once

#include "Core.h"

namespace Tree
{
	namespace Sap
	{
		class SapString
		{
		public:
			static SapString New( const char* InString );
			static SapString New( std::string_view InString );
			static void Free( SapString& InString );

			void Assign( std::string_view InString );

			operator std::string() const;

			bool operator==( const SapString& InOther ) const;
			bool operator==( std::string_view InOther ) const;

			SapChar* Data() { return m_String; }
			const SapChar* Data() const { return m_String; }

		private:
			SapChar* m_String = nullptr;
			Bool32 m_IsDisposed = false;
		};

		struct ScopedSapString
		{
			ScopedSapString( SapString InString )
				: m_String( InString )
			{
			}

			ScopedSapString& operator=( SapString InOther )
			{
				SapString::Free( m_String );
				m_String = InOther;
				return *this;
			}

			ScopedSapString& operator=( const ScopedSapString& InOther )
			{
				SapString::Free( m_String );
				m_String = InOther.m_String;
				return *this;
			}

			~ScopedSapString()
			{
				SapString::Free( m_String );
			}

			operator std::string() const { return m_String; }
			operator SapString() const { return m_String; }

			bool operator==( const ScopedSapString& InOther ) const
			{
				return m_String == InOther.m_String;
			}

			bool operator==( std::string_view InOther ) const
			{
				return m_String == InOther;
			}

		private:
			SapString m_String;
		};
	}
}
