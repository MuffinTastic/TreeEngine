﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Tree.Sap.Generated;

namespace Tree.Engine;

public static class TreeEngine
{
    internal static IEngineSystem IEngineSystem { get; } = Sys.Engine();
    public static EDomain s_Domain { get; } = IEngineSystem.GetDomain();

    
    public static bool IsClient => ( s_Domain & EDomain.EDOMAIN_CLIENT ) == EDomain.EDOMAIN_CLIENT;
    public static bool IsClientOnly => s_Domain == EDomain.EDOMAIN_CLIENT;
    public static bool IsEditorOnly => ( s_Domain & EDomain.EDOMAIN_EDITOR_ONLY ) == EDomain.EDOMAIN_EDITOR_ONLY;
    public static bool IsDedicatedServer => s_Domain == EDomain.EDOMAIN_DEDICATED_SERVER;

}