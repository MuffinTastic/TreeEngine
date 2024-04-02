using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Tree.Sap.Generated;

namespace Tree.Engine;

public static class CmdLine
{
    internal static ICmdLineSystem s_CmdLineSystem { get; } = Sys.CmdLine();

    public static bool GetFlag( string name )
    {
        return s_CmdLineSystem.GetFlag( name );
    }

    public static string GetOption( string name, string def )
    {
        return s_CmdLineSystem.GetStringOption( name, def );
    }

    public static int GetOption( string name, int def )
    {
        return s_CmdLineSystem.GetIntOption( name, def );
    }
}
