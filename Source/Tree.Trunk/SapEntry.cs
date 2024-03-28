using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Tree.Sap;

namespace Tree.Trunk;

public class SapEntry
{
    internal static unsafe delegate*<SapString, void> stringFunc;
    internal static unsafe delegate*<SapArray<float>, void> arrayFunc;


    [UnmanagedCallersOnly]
    public static void Entry( SapArray<nint> testpointers )
    {
        //Console.WriteLine( $"Hello from C#, {testpointers.Count()}" );
        //testpointers.Dispose();

        return;

        unsafe
        {
            stringFunc = (delegate*<SapString, void>) testpointers[0];
            arrayFunc = (delegate*< SapArray<float>, void >) testpointers[1];

            using SapString teststr = "Yeag! ユニコードいずぐれーと";
            stringFunc( teststr );

            SapArray<float> floats = new( 20 );
            for ( int i = 0; i < 20; ++i )
            {
                floats[i] = i;
            }

            arrayFunc( floats );

            floats.Dispose();
        }

    }
}
