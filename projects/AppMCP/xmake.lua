target("AppMCP")
    set_kind("binary")
    add_files("**.cpp")
    add_deps("AstHarness", "AstAgent", "AstCore", "AstSim", "AstMath", "AstUtil")
