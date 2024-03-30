global using static Tree.Engine.Global;

namespace Tree.Engine;

public static class Global
{
    public static Logger Log { get; } = LogSystem.CreateLogger( "Managed" );
}
