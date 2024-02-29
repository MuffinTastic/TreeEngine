include "PremakeScripts/CSharpTweaks.lua"
include "PremakeScripts/SolutionFiles.lua"

newoption {
    trigger = "with-opengl",
    description = "Force the use of OpenGL for rendering, regardless of platform"
}

local function defaultTarget()
    targetdir           "Bin/%{cfg.buildcfg}"
end

local function launcherTarget()
    targetdir           "Bin/%{cfg.buildcfg}"
end

local function engineTarget()
    targetdir           "Bin/%{cfg.buildcfg}/Engine"
end

local function toolTarget()
    targetdir           "ToolBin/%{cfg.buildcfg}"
end

local function workspaceLocation()
    location            "Build"
end

local function projectLocation()
    location            "Build/%{prj.name}"
end

local function clientBinary()
    defines             { "CLIENT" }
end

local function serverBinary()
    defines             { "SERVER" }
end


workspace "TreeEngine"
    configurations      { "Debug", "Release" }
    platforms           { "x64" }
    cppdialect          "C++20"
    dotnetframework     "net8.0"
    defaultTarget       ()
    workspaceLocation   ()

    solutionitems {
        ["Premake"] = {
            "../premake5.lua",
            ["Scripts"] = {
                "../Premake/**.lua"
            }
        }
    }

    filter { "platforms:x64" }
        architecture    "x86_64"

    filter { "system:Windows" }
        system          "windows"
        defines         { "WINDOWS" }
    
    filter { "system:Linux" }
        system          "linux"
        defines         { "LINUX" }

    filter "configurations:Debug"
        defines         { "DEBUG" }
        symbols         "On"

    filter "configurations:Release"
        defines         { "NDEBUG" }
        optimize        "On"


group "Engine"
    group "Engine/Managed"
        project "Tree.Trunk"
            kind                "SharedLib"
            language            "C#"
            engineTarget        ()
            projectLocation     ()

            files               { "Source/Tree.Trunk/**.cs" }

    group "Engine/Native"
        project "Tree.Root.Client"
            kind                "SharedLib"
            language            "C++"
            engineTarget        ()
            projectLocation     ()
            clientBinary        ()

            files               { "Source/Tree.Root/**.h", "Source/Tree.Root/**.cpp" }

        project "Tree.Root.Server"
            kind                "SharedLib"
            language            "C++"
            engineTarget        ()
            projectLocation     ()
            serverBinary        ()

            files               { "Source/Tree.Root/**.h", "Source/Tree.Root/**.cpp" }


group "Launchers"
    project "Tree.Launcher.Client"
        kind                "ConsoleApp"
        language            "C++"
        launcherTarget      ()
        projectLocation     ()
        clientBinary        ()

        files               { "Source/Tree.Launcher/**.h", "Source/Tree.Launcher/**.cpp" }

    project "Tree.Launcher.Server"
        kind                "ConsoleApp"
        language            "C++"
        launcherTarget      ()
        projectLocation     ()
        serverBinary        ()

        files               { "Source/Tree.Launcher/**.h", "Source/Tree.Launcher/**.cpp" }


group "Tools"
    project "Tree.GlueGen"
        kind                "ConsoleApp"
        language            "C#"
        toolTarget          ()
        projectLocation     ()
        nuget               { "Antlr4.Runtime.Standard:4.13.1" }

        files               { "Source/Tree.GlueGen/**.cs" }