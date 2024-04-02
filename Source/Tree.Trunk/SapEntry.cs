using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Tree.Engine;
using Tree.Sap.Generated;

namespace Tree.Trunk;

public class SapEntry
{
    public static void RunEvent( int evtInt )
    {
        var evt = (EManagedHostEvent) evtInt;

        switch ( evt )
        {
            case EManagedHostEvent.EMHEVENT_STARTUP:
                Startup();
                break;
            case EManagedHostEvent.EMHEVENT_SHUTDOWN:
                Shutdown();
                break;
        }
    }

    private static void Startup()
    {
        Log.Info( $"Domain: {TreeEngine.s_Domain}" );
        Log.Info( $"IsClient: {TreeEngine.IsClient}" );
        Log.Info( $"IsClientOnly: {TreeEngine.IsClientOnly}" );
        Log.Info( $"IsEditorOnly: {TreeEngine.IsEditorOnly}" );
        Log.Info( $"IsDedicatedServer: {TreeEngine.IsDedicatedServer}" );

        Log.Info( "" );
        Log.Info( "Console log entries:" );

        foreach ( var entry in LogSystem.GetConsoleLogEntries() )
        {
            Log.Info( $" - [{entry.timestamp.ToLocalTime()}] [{entry.loggerName}] [{entry.logLevel}] {entry.text}" );
        }

        Log.Info( "" );
    }

    private static void Shutdown()
    {

    }
}