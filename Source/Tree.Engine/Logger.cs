using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Tree.Engine;

public class Logger
{
    private Sap.Generated.ILogger _logger;

    internal Logger( Sap.Generated.ILogger logger )
    {
        _logger = logger;
    }

    public void Info( string text,
        [CallerMemberName] string memberName = "",
        [CallerFilePath] string sourceFilePath = "",
        [CallerLineNumber] int sourceLineNumber = 0 )
    {
        _logger.InternalInfo( sourceFilePath, sourceLineNumber, memberName, text );
    }

    public void Warning( string text,
        [CallerMemberName] string memberName = "",
        [CallerFilePath] string sourceFilePath = "",
        [CallerLineNumber] int sourceLineNumber = 0 )
    {
        _logger.InternalWarning( sourceFilePath, sourceLineNumber, memberName, text );
    }

    public void Error( string text,
        [CallerMemberName] string memberName = "",
        [CallerFilePath] string sourceFilePath = "",
        [CallerLineNumber] int sourceLineNumber = 0 )
    {
        _logger.InternalError( sourceFilePath, sourceLineNumber, memberName, text );
    }
}
