require "vstudio"

local p = premake
local vstudio = p.vstudio
local dotnetbase  = p.vstudio.dotnetbase
local project = p.project
local config = p.config
local fileconfig = p.fileconfig
local dotnet = p.tools.dotnet

premake.override(premake.vstudio.dotnetbase, "projectProperties", function(base, prj)
    _p(1,'<PropertyGroup>')
    local cfg = project.getfirstconfig(prj)
    p.callArray(dotnetbase.elements.projectProperties, cfg)
    
    -- fix compile
    local platforms = prj.platforms
    if platforms ~= nil and #platforms > 0 then
        table.replace(platforms, 'Win32', 'x86')
        table.replace(platforms, 'x86_64', 'x64')
        _p(2, '<Platforms>%s</Platforms>', table.concat(platforms, ';'))
    else
        _p(2, '<Platforms>AnyCPU</Platforms>')
    end

    _p(1,'</PropertyGroup>')
end)

premake.override(premake.vstudio.dotnetbase, "outputProps", function(base, cfg)
    local outdir = vstudio.path(cfg, cfg.buildtarget.directory)
    
    -- make binaries output to targetdir directly instead of appending net8.0
    _x(2,'<OutDir>%s\\</OutDir>', outdir)

    -- Want to set BaseIntermediateOutputPath because otherwise VS will create obj/
    -- anyway. But VS2008 throws up ominous warning if present.
    local objdir = vstudio.path(cfg, cfg.objdir)
    if _ACTION > "vs2008" and not dotnetbase.isNewFormatProject(cfg) then
        _x(2,'<BaseIntermediateOutputPath>%s\\</BaseIntermediateOutputPath>', objdir)
        _p(2,'<IntermediateOutputPath>$(BaseIntermediateOutputPath)</IntermediateOutputPath>')
    else
        _x(2,'<IntermediateOutputPath>%s\\</IntermediateOutputPath>', objdir)
    end
end)