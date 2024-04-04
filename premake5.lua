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

    flags { "MultiProcessorCompile" }

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


function thirdpartylib( names )
    -- my precious little hack...
    if type(names) == "table" then
        return "ThirdParty/Lib/%{cfg.buildcfg:gsub('Debug', '" .. names.debug .. "'):gsub('Release', '" .. names.release .. "')}"
    else
        return "ThirdParty/Lib/" .. names
    end
end

group "Managed"
    treeproject {
        name = "Tree.Sap",
        language = "C#",
        kind = "SharedLib",
        target = "Engine",
        generateruntimecfg = true,
        allowunsafeblocks = true
    }

    treeproject {
        name = "Tree.Engine",
        language = "C#",
        kind = "SharedLib",
        target = "Engine",
        allowunsafeblocks = true,
        links = { "Tree.Sap" },
        dependson = { "Tree.SapGen" },
        nuget = {
            "Microsoft.Build:17.9.5",
            "Microsoft.CodeAnalysis.CSharp:4.9.2",
            "NuGet.Protocol:6.9.1"
        },
    }

    treeproject {
        name = "Tree.Trunk",
        language = "C#",
        kind = "SharedLib",
        target = "Engine",
        allowunsafeblocks = true,
        nocopylocal = true,
        links = { "Tree.Sap", "Tree.Engine" }
    }

group "Native"
    treeproject {
            name = "Tree.Root",
            language = "C++",
            kind = "SharedLib",
            target = "Engine",
            links = {
                "%{cfg.buildtarget.directory}/Tree.NativeCommon",
                thirdpartylib{ debug="fmtd", release="fmt" }
            },
            dependson = { "Tree.NativeCommon", "Tree.SapGen" }
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
            "%{cfg.buildtarget.directory}/Engine/Tree.NativeCommon",
            thirdpartylib{ debug="fmtd", release="fmt" },
        },
        dependson = { "Tree.NativeCommon", "Tree.Root", "Tree.Trunk" },
        defines = { "LAUNCHER" },
        postbuildcommands = { "{COPYDIR} %[ThirdParty/Bin] %[%{cfg.buildtarget.directory}/Engine]" }
    }

group "Tools"
    treeproject {
        name = "Tree.SapGen",
        language = "C#",
        kind = "ConsoleApp",
        target = "Tools",
        allowunsafeblocks = true,
        nuget = {
            "ClangSharp:17.0.1",
            "libClangSharp.runtime.win-x64:17.0.4"
        },
        postbuildcommands = {
            "{CHDIR} %[%{wks.location}]",
            "%[%{cfg.buildtarget.directory}%{cfg.buildtarget.basename}]"
        },
        alwaysrunpostbuild = true
    }

workspace "*"
        -- Exclude os-specific files
    filter { "system:linux", "files:**_win.*" }
        flags { "ExcludeFromBuild" }
    filter { "system:windows", "files:**_linux.*"}
        flags { "ExcludeFromBuild"}