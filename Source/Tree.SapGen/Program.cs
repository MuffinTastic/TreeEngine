using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tree.SapGen;

public static class Program
{
    public static void Main( string[] args )
    {
        if ( args.Length < 1 )
        {
            Console.WriteLine( "Need Source path" );
            return;
        }

        Console.WriteLine( $"Running, received {args[0]}" );
    }


}
