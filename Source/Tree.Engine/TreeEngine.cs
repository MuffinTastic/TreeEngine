using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Tree.Sap.Generated;

namespace Tree.Engine;

public static class TreeEngine
{
    internal static IEngineSystem s_EngineSystem { get; } = Sys.Engine();
    internal static EDomain s_Domain { get; } = s_EngineSystem.GetDomain();

    
    public static bool IsGame => ( s_Domain & EDomain.EDOMAIN_GAME ) == EDomain.EDOMAIN_GAME;
    public static bool IsGameOnly => s_Domain == EDomain.EDOMAIN_GAME;
    public static bool IsEditorOnly => ( s_Domain & EDomain.EDOMAIN_EDITOR_ONLY ) == EDomain.EDOMAIN_EDITOR_ONLY;
    public static bool IsDedicatedServer => s_Domain == EDomain.EDOMAIN_DEDICATED_SERVER;

    public static bool IsDevMode { get; } = CmdLine.GetFlag( "dev" );

    public static string ExecutableFile { get; } = Platform.GetExecutableFileUTF8();
    public static string ExecutableDirectory { get; } = Platform.GetExecutableDirectoryUTF8();
    public static string EngineDirectory { get; } = Platform.GetEngineDirectoryUTF8();
    public static string LogDirectory { get; } = Platform.GetLogDirectoryUTF8();
}
