#pragma once

#include "Memory.h"

namespace Tree
{
	namespace Sap
	{		
		template<typename TValue>
		class SapArray
		{
		public:
			static SapArray New( int32_t InLength )
			{
				SapArray<TValue> result;
				if ( InLength > 0 )
				{
					result.m_Ptr = static_cast<TValue*>( Memory::AllocHGlobal( InLength * sizeof( TValue ) ) );
					result.m_Length = InLength;
				}
				return result;
			}

			static SapArray New( const std::vector<TValue>& InValues )
			{
				SapArray<TValue> result;

				if ( !InValues.empty() )
				{
					result.m_Ptr = static_cast<TValue*>( Memory::AllocHGlobal( InValues.size() * sizeof( TValue ) ) );
					result.m_Length = static_cast<int32_t>( InValues.size() );
					memcpy( result.m_Ptr, InValues.data(), InValues.size() * sizeof( TValue ) );
				}

				return result;
			}

			static SapArray New( std::initializer_list<TValue> InValues )
			{
				SapArray result;

				if ( InValues.size() > 0 )
				{
					result.m_Ptr = static_cast<TValue*>( Memory::AllocHGlobal( InValues.size() * sizeof( TValue ) ) );
					result.m_Length = static_cast<int32_t>( InValues.size() );
					memcpy( result.m_Ptr, InValues.begin(), InValues.size() * sizeof( TValue ) );
				}

				return result;
			}

			static void Free( SapArray InArray )
			{
				if ( !InArray.m_Ptr || InArray.m_Length == 0 )
					return;

				Memory::FreeHGlobal( InArray.m_Ptr );
				InArray.m_Ptr = nullptr;
				InArray.m_Length = 0;
			}

			void Assign( const SapArray& InOther )
			{
				memcpy( m_Ptr, InOther.m_Ptr, InOther.m_Length * sizeof( TValue ) );
			}

			bool IsEmpty() const { return m_Length == 0 || m_Ptr == nullptr; }

			TValue& operator[]( size_t InIndex ) { return m_Ptr[InIndex]; }
			const TValue& operator[]( size_t InIndex ) const { return m_Ptr[InIndex]; }

			size_t Length() const { return m_Length; }
			size_t ByteLength() const { return m_Length * sizeof( TValue ); }

			TValue* Data() { return m_Ptr; }
			const TValue* Data() const { return m_Ptr; }

			TValue* begin() { return m_Ptr; }
			TValue* end() { return m_Ptr + m_Length; }

			const TValue* begin() const { return m_Ptr; }
			const TValue* end() const { return m_Ptr + m_Length; }

			const TValue* cbegin() const { return m_Ptr; }
			const TValue* cend() const { return m_Ptr + m_Length; }

		private:
			TValue* m_Ptr = nullptr;
			int32_t m_Length = 0;
			Bool32 m_IsDisposed = false;
		};

	}
}