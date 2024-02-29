using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Tree.Trunk;

class Program
{
    delegate double MyCallback( double x, double y );

    [DllImport( "Tree.Root.dll", CallingConvention = CallingConvention.Cdecl )]
    static extern double dostuff( double x, double y, [MarshalAs( UnmanagedType.FunctionPtr )] MyCallback func );

    static void Main( string[] args )
    {
        double z = dostuff( 1, 2, ( x, y ) => 45 );
    }
}