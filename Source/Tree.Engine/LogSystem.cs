using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Tree.Engine;

public static class LogSystem
{
    internal static Sap.Generated.ILogSystem ILogSystem { get; } = Sap.Generated.Sys.Log();

    private static Dictionary<string, Logger> s_LoggerCache = new();

    public static Logger CreateLogger( string name )
    {
        bool exists = s_LoggerCache.TryGetValue( name, out Logger? logger );

        if ( exists )
        {
            return logger!;
        }
        else
        {
            logger = new Logger( ILogSystem.CreateLoggerSap( name ) );
            s_LoggerCache.Add( name, logger );
            return logger;
        }
    }
}
