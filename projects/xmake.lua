set_group("projects")
includes("*/xmake.lua")
add_deps("AstUtil", "AstCore", "AstMath", "AstOpt", "AstSim", "AstTest", "AstCalc", "AstSPICE", "AstMock", "AstHarness", "AstAgent")
if is_plat("windows") then
    add_cxflags("/utf-8")
end