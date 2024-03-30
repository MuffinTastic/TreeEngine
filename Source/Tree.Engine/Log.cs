using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Tree.Engine;

public static class Log
{
    private static Sap.Generated.ILogSystem? s_LogSystem = null;
    private static Sap.Generated.ILogger? s_DefaultLogger = null;

    internal static Sap.Generated.ILogSystem LogSystem
    {
        get
        {
            if ( s_LogSystem is null )
                s_LogSystem = Sap.Generated.Sys.Log();
            return s_LogSystem!;
        }
    }

    internal static Sap.Generated.ILogger DefaultLogger
    {
        get
        {
            if ( s_DefaultLogger is null )
                s_DefaultLogger = LogSystem.Helper_ConvToILogger();
            return s_DefaultLogger!;
        }
    }

    public static void Info( string text,
        [CallerMemberName] string memberName = "",
        [CallerFilePath] string sourceFilePath = "",
        [CallerLineNumber] int sourceLineNumber = 0 )
    {
        DefaultLogger.InternalInfo( sourceFilePath, sourceLineNumber, memberName, text );
    }
}
