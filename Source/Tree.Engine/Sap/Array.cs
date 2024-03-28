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

using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Tree.Sap;

public sealed class SapArrayEnumerator<T> : IEnumerator<T>
{
    private readonly T[] m_Elements;
    private int m_Index = -1;

    public SapArrayEnumerator( T[] elements )
    {
        m_Elements = elements;
    }

    public bool MoveNext()
    {
        m_Index++;
        return m_Index < m_Elements.Length;
    }

    void IEnumerator.Reset() => m_Index = -1;
    void IDisposable.Dispose()
    {
        m_Index = -1;
        GC.SuppressFinalize( this );
    }

    object IEnumerator.Current => Current!;

    public T Current => m_Elements[m_Index];

}

[StructLayout( LayoutKind.Sequential, Pack = 1 )]
public struct SapArray<T> : IDisposable, IEnumerable<T>
{
    private readonly IntPtr m_NativeArray;
    private readonly int m_NativeLength;

    private SapBool32 m_IsDisposed;

    public int Length => m_NativeLength;

    public SapArray( int InLength )
    {
        m_NativeArray = Marshal.AllocHGlobal( InLength * Marshal.SizeOf<T>() );
        m_NativeLength = InLength;
    }

    public SapArray( [DisallowNull] T?[] InValues )
    {
        m_NativeArray = Marshal.AllocHGlobal( InValues.Length * Marshal.SizeOf<T>() );
        m_NativeLength = InValues.Length;

        for ( int i = 0; i < m_NativeLength; i++ )
        {
            var elem = InValues[i];

            if ( elem == null )
                continue;

            Marshal.StructureToPtr( elem, IntPtr.Add( m_NativeArray, i * Marshal.SizeOf<T>() ), false );
        }
    }

    internal SapArray( IntPtr InArray, int InLength )
    {
        m_NativeArray = InArray;
        m_NativeLength = InLength;
    }

    public T[] ToArray()
    {
        Span<T> data = Span<T>.Empty;

        if ( m_NativeArray != IntPtr.Zero && m_NativeLength > 0 )
        {
            unsafe { data = new Span<T>( m_NativeArray.ToPointer(), m_NativeLength ); }
        }

        return data.ToArray();
    }

    public Span<T> ToSpan()
    {
        unsafe { return new Span<T>( m_NativeArray.ToPointer(), m_NativeLength ); }
    }

    public ReadOnlySpan<T> ToReadOnlySpan() => ToSpan();

    public void Dispose()
    {
        if ( !m_IsDisposed )
        {
            Marshal.FreeHGlobal( m_NativeArray );
            m_IsDisposed = true;
        }

        GC.SuppressFinalize( this );
    }

    public IEnumerator<T> GetEnumerator() => new SapArrayEnumerator<T>( this );
    IEnumerator IEnumerable.GetEnumerator() => new SapArrayEnumerator<T>( this );

    public T? this[int InIndex]
    {
        get => Marshal.PtrToStructure<T>( IntPtr.Add( m_NativeArray, InIndex * Marshal.SizeOf<T>() ) );
        set => Marshal.StructureToPtr<T>( value!, IntPtr.Add( m_NativeArray, InIndex * Marshal.SizeOf<T>() ), false );
    }

    public static implicit operator T[]( SapArray<T> InArray ) => InArray.ToArray();

}
