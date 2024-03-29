require "vstudio"

local p = premake
local vstudio = p.vstudio
local dotnetbase  = p.vstudio.dotnetbase
local project = p.project
local config = p.config
local fileconfig = p.fileconfig
local dotnet = p.tools.dotnet

premake.api.register {
    name = "generateruntimecfg",
    scope = "config",
    kind = "boolean"
}

premake.api.register {
    name = "allowunsafeblocks",
    scope = "config",
    kind = "boolean"
}

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

    if cfg.generateruntimecfg then
        _p(2, '<GenerateRuntimeConfigurationFiles>true</GenerateRuntimeConfigurationFiles>')
    end

    if cfg.allowunsafeblocks then
        _p(2, '<AllowUnsafeBlocks>true</AllowUnsafeBlocks>')
    end

    _p(2, '<Nullable>enable</Nullable>')

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

premake.override(premake.vstudio.dotnetbase, "buildEvents", function(base, prj)
    local function output(name, steps)
        if #steps > 0 then
            steps = os.translateCommandsAndPaths(steps, prj.basedir, prj.location)
            steps = table.implode(steps, "", "", "\r\n")
            steps = premake.vstudio.vs2005.esc(steps)

            _p(1, '<Target Name="%sBuild" AfterTargets="%sBuildEvent">', name, name)
                _p(2,'<Exec Command="%s" />', steps)
            _p(1, '</Target>')
        end
    end

    local cfg = project.getfirstconfig(prj)
    if #cfg.prebuildcommands > 0 or #cfg.postbuildcommands > 0 then
        output("Pre", cfg.prebuildcommands)
        output("Post", cfg.postbuildcommands)
    end
end)
