﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Tree.Sap.Generated;
using Tree.Sap.Interop;

namespace Tree.Engine;

public enum EConsoleLogLevel : int
{
    Trace       = 0,
    Debug       = 1,
    Info        = 2,
    Warning     = 3,
    Error       = 4,
    Critical    = 5
};

public struct ConsoleLogEntry
{
    public string loggerName;
    public EConsoleLogLevel logLevel;
    public DateTime timestamp;
    public string text;
}

public static class LogSystem
{
    internal static ILogSystem s_LogSystem { get; } = Sys.Log();

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
            logger = new Logger( s_LogSystem.CreateLogger( name ) );
            s_LoggerCache.Add( name, logger );
            return logger;
        }
    }

    public static List<ConsoleLogEntry> GetConsoleLogEntries()
    {
        List<ConsoleLogEntry> entries = new();

        using NativeArray<ConsoleLogEntrySap> sapEntries = s_LogSystem.GetConsoleLogHistorySap();

        foreach ( var sapEntry in sapEntries )
        {
            ConsoleLogEntry entry = new ConsoleLogEntry()
            {
                loggerName = sapEntry.loggerName!,
                logLevel = (EConsoleLogLevel) sapEntry.logLevel,
                timestamp = DateTime.UnixEpoch.AddMilliseconds( sapEntry.timestamp ),
                text = sapEntry.text!
            };

            entries.Add( entry );
        }

        return entries;
    }
}
