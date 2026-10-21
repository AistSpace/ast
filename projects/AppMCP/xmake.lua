target("AppMCP")
    set_kind("binary")
    add_files("**.cpp")
    add_deps("AstAI", "AstCore", "AstSim", "AstMath", "AstUtil")
