require "vstudio"

local p = premake
local msc = p.tools.msc
local v = msc.shared.vectorextensions
local m = p.vstudio.vc2010

premake.api.addAllowed("vectorextensions", { "AVX512" })
v.AVX512 = "/arch:AVX512"

premake.override(premake.vstudio.vc2010, "enableEnhancedInstructionSet", function(base, cfg, condition)
    local v
    local x = cfg.vectorextensions
    if x == "AVX" and _ACTION > "vs2010" then
        v = "AdvancedVectorExtensions"
    elseif x == "AVX2" and _ACTION > "vs2012" then
        v = "AdvancedVectorExtensions2"
    elseif x == "AVX512" and _ACTION > "vs2012" then
        v = "AdvancedVectorExtensions512"
    elseif cfg.architecture ~= "x86_64" then
        if x == "SSE2" or x == "SSE3" or x == "SSSE3" or x == "SSE4.1" or x == "SSE4.2" then
            v = "StreamingSIMDExtensions2"
        elseif x == "SSE" then
            v = "StreamingSIMDExtensions"
        elseif x == "IA32" and _ACTION > "vs2010" then
            v = "NoExtensions"
        end
    end
    if v then
        m.element('EnableEnhancedInstructionSet', condition, v)
    end
end)
