// Modified from https://github.com/StudioCherno/Coral

// MIT License
// 
// Copyright( c ) 2023 Studio Cherno
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

#include "common.h"
#include "memory.h"

namespace Tree
{
	namespace Sap
	{
		template<typename TValue>
		class SapArray
		{
		public:
			static SapArray New( int32_t length )
			{
				SapArray<TValue> result;
				if ( length > 0 )
				{
					result.m_ptr = static_cast<TValue*>( AllocHGlobal( length * sizeof( TValue ) ) );
					result.m_length = length;
				}
				return result;
			}

			static SapArray New( const std::vector<TValue>& values )
			{
				SapArray<TValue> result;

				if ( !values.empty() )
				{
					result.m_ptr = static_cast<TValue*>( AllocHGlobal( values.size() * sizeof( TValue ) ) );
					result.m_length = static_cast<int32_t>( values.size() );
					std::memcpy( result.m_ptr, values.data(), values.size() * sizeof( TValue ) );
				}

				return result;
			}

			static SapArray New( std::initializer_list<TValue> values )
			{
				SapArray result;

				if ( values.size() > 0 )
				{
					result.m_ptr = static_cast<TValue*>( AllocHGlobal( values.size() * sizeof( TValue ) ) );
					result.m_length = static_cast<int32_t>( values.size() );
					std::memcpy( result.m_ptr, values.begin(), values.size() * sizeof( TValue ) );
				}

				return result;
			}

			static void Free( SapArray array )
			{
				if ( !array.m_ptr || array.m_length == 0 )
					return;

				FreeHGlobal( array.m_ptr );
				array.m_ptr = nullptr;
				array.m_length = 0;
			}

			void Assign( const SapArray& other )
			{
				memcpy( m_ptr, other.m_ptr, other.m_length * sizeof( TValue ) );
			}

			bool IsEmpty() const { return m_length == 0 || m_ptr == nullptr; }

			TValue& operator[]( size_t index ) { return m_ptr[index]; }
			const TValue& operator[]( size_t index ) const { return m_ptr[index]; }

			size_t Length() const { return m_length; }
			size_t ByteLength() const { return m_length * sizeof( TValue ); }

			TValue* Data() { return m_ptr; }
			const TValue* Data() const { return m_ptr; }

			TValue* begin() { return m_ptr; }
			TValue* end() { return m_ptr + m_length; }

			const TValue* begin() const { return m_ptr; }
			const TValue* end() const { return m_ptr + m_length; }

			const TValue* cbegin() const { return m_ptr; }
			const TValue* cend() const { return m_ptr + m_length; }

		private:
			TValue* m_ptr = nullptr;
			int32_t m_length = 0;
			SapBool32 m_isDisposed = false;
		};
	}
}