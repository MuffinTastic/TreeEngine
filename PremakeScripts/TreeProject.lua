function treeproject(properties)
    function treeprojectinner()
        project(properties.nameoverride or properties.name)
            kind(properties.kindoverride or properties.kind)
            language(properties.language)
            sourcedir = "%{wks.location}/" .. properties.name .. "/"
            location(sourcedir)
            
            linksvalue = properties.linksoverride or properties.links
            if linksvalue then
                links(linksvalue)
            end
            
            if properties.target == "Tools" then
                targetdir "ToolBin/%{cfg.buildcfg}"
            elseif properties.target == "Engine" then
                targetdir "Bin/%{cfg.buildcfg}/Engine"
            else
                targetdir "Bin/%{cfg.buildcfg}"
                debugdir "Bin/%{cfg.buildcfg}"
            end
        
            if properties.language == "C#" then
                files { sourcedir .. "**.cs" }
            elseif properties.language == "C++" then
                files { sourcedir .. "**.h", sourcedir .. "**.cpp" }
            end

            if properties.defines then
                defines(properties.defines)
            end

            if properties.dependson then
                dependson(properties.dependson)
            end

            if properties.postbuildcommands then
                postbuildcommands(properties.postbuildcommands)
            end

            if properties.generateruntimecfg then
                generateruntimecfg "On"
            end

            if properties.allowunsafeblocks then
                allowunsafeblocks "On"
            end

            if properties.nuget then
                nuget(properties.nuget)
            end

            if properties.alwaysrunpostbuild then
                alwaysrunpostbuild "On"
            end

            if properties.nocopylocal then
                flags { "NoCopyLocal" }
            end

            if properties.copynugetdeps then
                copynugetdeps "On"
            end
    end



    if properties.splitdomain then
        domainkind = {}
        domainlinks = {}

        if type(properties.kind) == "table" then
            domainkind.editor = properties.kind.editor
            domainkind.game = properties.kind.game
            domainkind.server = properties.kind.server
        end

        if properties.links then
            if properties.links.editor then
                domainkind.editor = properties.links.editor
                domainkind.game = properties.links.game
                domainkind.server = properties.links.server
            end
        end

        properties.nameoverride = properties.name .. " (Editor)"
        properties.kindoverride = domainkind.editor or nil
        properties.linksoverride = domainlinks.editor or nil
        treeprojectinner()
            targetname(properties.name .. ".Editor")
            defines { "GAME", "EDITOR" }
            filter { "files:**_serveronly.*"}
                flags { "ExcludeFromBuild"}

        properties.nameoverride = properties.name .. " (Game)"
        properties.kindoverride = domainkind.game or nil
        properties.linksoverride = domainlinks.game or nil
        treeprojectinner()
            targetname(properties.name .. ".Game")
            defines { "GAME" }
            filter { "files:**_editoronly.*" }
                flags { "ExcludeFromBuild" }
            filter { "files:**_serveronly.*"}
                flags { "ExcludeFromBuild"}

        properties.nameoverride = properties.name .. " (DedicatedServer)"
        properties.kindoverride = domainkind.server or nil
        properties.linksoverride = domainlinks.server or nil
        treeprojectinner()
            targetname(properties.name .. ".DedicatedServer")
            defines { "DEDICATED_SERVER" }
            filter { "files:**_editoronly.*" }
                flags { "ExcludeFromBuild" }
            filter { "files:**_gameonly.*"}
                flags { "ExcludeFromBuild"}
    else
        treeprojectinner()
    end
end