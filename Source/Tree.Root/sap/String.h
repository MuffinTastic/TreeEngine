#pragma once

#include "Core.h"

namespace Tree
{
	namespace Sap
	{
		class String
		{
		public:
			static String New( const char* InString );
			static String New( std::string_view InString );
			static void Free( String& InString );

			void Assign( std::string_view InString );

			operator std::string() const;

			bool operator==( const String& InOther ) const;
			bool operator==( std::string_view InOther ) const;

			SapChar* Data() { return m_String; }
			const SapChar* Data() const { return m_String; }

		private:
			SapChar* m_String = nullptr;
			Bool32 m_IsDisposed = false;
		};

		struct ScopedString
		{
			ScopedString( String InString )
				: m_String( InString )
			{
			}

			ScopedString& operator=( String InOther )
			{
				String::Free( m_String );
				m_String = InOther;
				return *this;
			}

			ScopedString& operator=( const ScopedString& InOther )
			{
				String::Free( m_String );
				m_String = InOther.m_String;
				return *this;
			}

			~ScopedString()
			{
				String::Free( m_String );
			}

			operator std::string() const { return m_String; }
			operator String() const { return m_String; }

			bool operator==( const ScopedString& InOther ) const
			{
				return m_String == InOther.m_String;
			}

			bool operator==( std::string_view InOther ) const
			{
				return m_String == InOther;
			}

		private:
			String m_String;
		};
	}
}
