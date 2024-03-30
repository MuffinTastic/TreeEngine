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

    internal static Sap.Generated.ILogSystem LogSystem
    {
        get
        {
            if ( s_LogSystem is null )
                s_LogSystem = Sap.Generated.Sys.Log();
            return s_LogSystem!;
        }
    }

    public static void Info( string text,
        [CallerMemberName] string memberName = "",
        [CallerFilePath] string sourceFilePath = "",
        [CallerLineNumber] int sourceLineNumber = 0 )
    {
        LogSystem.InternalInfo( sourceFilePath, sourceLineNumber, memberName, text );
    }
}
