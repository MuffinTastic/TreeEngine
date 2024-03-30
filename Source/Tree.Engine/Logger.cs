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

    public void Info( object obj,
        [CallerMemberName] string memberName = "",
        [CallerFilePath] string sourceFilePath = "",
        [CallerLineNumber] int sourceLineNumber = 0 )
    {
        _logger.InternalInfo( sourceFilePath, sourceLineNumber, memberName, obj.ToString() );
    }

    public void Warning( object obj,
        [CallerMemberName] string memberName = "",
        [CallerFilePath] string sourceFilePath = "",
        [CallerLineNumber] int sourceLineNumber = 0 )
    {
        _logger.InternalWarning( sourceFilePath, sourceLineNumber, memberName, obj.ToString() );
    }

    public void Error( object obj,
        [CallerMemberName] string memberName = "",
        [CallerFilePath] string sourceFilePath = "",
        [CallerLineNumber] int sourceLineNumber = 0 )
    {
        _logger.InternalError( sourceFilePath, sourceLineNumber, memberName, obj.ToString() );
    }
}
