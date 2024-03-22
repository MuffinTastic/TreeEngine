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
        defines         { "WINDOWS", "WIN32", "_WIN32" }
    
    filter { "system:linux" }
        defines         { "LINUX" }

    filter "configurations:Debug"
        defines         { "DEBUG" }
        symbols         "On"

    filter "configurations:Release"
        defines         { "NDEBUG" }
        optimize        "On"

    filter {}


function thirdpartylib( name )
    -- my precious little hack...
    return "ThirdParty/Lib/" .. name .. "%{cfg.buildcfg:gsub('Debug', 'd'):gsub('Release', '')}"
end

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
            links = {
                thirdpartylib( "fmt" ),
                "%{cfg.buildtarget.directory}/Tree.NativeCommon"
            },
            dependson = { "Tree.NativeCommon" },
        }

    treeproject {
            name = "Tree.NativeCommon",
            language = "C++",
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
        links = {
            thirdpartylib( "fmt" ),
            "%{cfg.buildtarget.directory}/Engine/Tree.NativeCommon"
        },
        dependson = { "Tree.NativeCommon", "Tree.Root", "Tree.Trunk" },
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