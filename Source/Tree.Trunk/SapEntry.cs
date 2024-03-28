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
    internal static unsafe delegate*<SapString, void> TestFunc;

    [UnmanagedCallersOnly]
    public static void Entry( nint test )
    {
        Console.WriteLine( $"Hello from C#, {test}" );

        SapString teststr = "Yeag! ユニコードいずぐれーと";

        unsafe
        {
            TestFunc = (delegate*<SapString, void>) test;

            TestFunc( teststr );
        }
    }
}
