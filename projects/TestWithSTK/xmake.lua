target("TestWithSTK")
    set_kind("binary")
    add_files("**.cpp")
    add_deps("AstTest", "AstUtil")
    if is_plat("windows") then 
        add_syslinks("shell32")
    end