using System;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Glue.Generated
{
    [StructLayout( LayoutKind.Sequential )]
    public struct TestData
    {
        [MarshalAs( UnmanagedType.ByValArray, SizeConst = 2 )]
        public float[] something;
        public int somethingElse;
        public uint blah;
    }

    class TestSystem
    {
        public delegate int InitializeCategoryDelegate(
            int count,
            [In] [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStruct, SizeParamIndex = 0)] nint[] functions
        );

        internal static int InitializeCategory( string category, IntPtr[] functions, int amount )
        {
            return 0;
        }
    }

}