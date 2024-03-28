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
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Tree.Sap;

// Use IDisposable interface if this is intended to be owned by managed code
[StructLayout( LayoutKind.Sequential )]
public struct SapString : IDisposable
{
    internal IntPtr m_NativeString;
    private SapBool32 m_IsDisposed;

    public void Dispose()
    {
        if ( !m_IsDisposed )
        {
            if ( m_NativeString != IntPtr.Zero )
            {
                Marshal.FreeCoTaskMem( m_NativeString );
                m_NativeString = IntPtr.Zero;
            }

            m_IsDisposed = true;
        }

        GC.SuppressFinalize( this );
    }

    public override string? ToString() => this;

    public static SapString Null() => new SapString() { m_NativeString = IntPtr.Zero };

    public static implicit operator SapString( string? InString ) => new() { m_NativeString = Marshal.StringToCoTaskMemAuto( InString ) };
    public static implicit operator string?( SapString InString ) => Marshal.PtrToStringAuto( InString.m_NativeString );
}