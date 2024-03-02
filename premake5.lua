include "PremakeScripts/CSharpTweaks.lua"
include "PremakeScripts/CleanVS.lua"
include "PremakeScripts/TreeProject.lua"


workspace "TreeEngine"
    configurations      { "Debug", "Release" }
    platforms           { "x64" }
    cppdialect          "C++20"
    dotnetframework     "net8.0"
    location            "Source"

    objdir              "Build"
    includedirs         { "%{wks.location}", "ThirdParty/Include" }

    filter { "platforms:x64" }
        architecture    "x86_64"

    filter { "system:windows" }
        defines         { "WINDOWS", "WIN32" }
    
    filter { "system:linux" }
        defines         { "LINUX" }

    filter "configurations:Debug"
        defines         { "DEBUG" }
        symbols         "On"

    filter "configurations:Release"
        defines         { "NDEBUG" }
        optimize        "On"

    filter {}


group "Managed"
    treeproject {
        name = "Tree.Trunk",
        language = "C#",
        kind = "SharedLib",
        target = "Engine"
    }
    links { "Tree.Interop" }

    treeproject {
        name = "Tree.Interop",
        language = "C#",
        kind = "SharedLib",
        target = "Engine"
    }

group "Native"
    treeproject {
            name = "Tree.Root",
            language = "C++",
            kind = "SharedLib",
            target = "Engine",
            links = { "Tree.NativeCommon" }
        }

    treeproject {
            name = "Tree.NativeCommon",
            language = "C++",
            splitdomain = false,
            kind = "StaticLib",
            target = "Engine"
        }

group "Native/Launchers"
    treeproject {
        name = "Tree.Launcher",
        language = "C++",
        splitdomain = true,
        kind = {
            editor = "WindowedApp",
            client = "WindowedApp",
            server = "ConsoleApp"
        },
        links = { "Tree.NativeCommon" },
        defines = { "LAUNCHER" }
    }

group "Tools"
    treeproject {
        name = "Tree.GlueGen",
        language = "C#",
        kind = "ConsoleApp",
        target = "Tools"
    }
        nuget { "Antlr4.Runtime.Standard:4.13.1" }


workspace "*"
        -- Exclude os-specific files
    filter { "system:linux", "files:**_win.*" }
        flags { "ExcludeFromBuild" }
    filter { "system:windows", "files:**_linux.*"}
        flags { "ExcludeFromBuild"}