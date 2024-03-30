using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Tree.Engine;

namespace Tree.Trunk;

public class SapEntry
{
    public static void Entry()
    {
        Log.Blubber();
    }
}

public static class LoggerExtensions
{
    public static void Blubber( this Logger logger )
    {
        logger.Info( "Oh yeah!!!! ユニコード" );
    }
}