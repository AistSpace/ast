xpack("ast")
    set_homepage("https://space-ast.github.io/ast")
    set_title("航天仿真算法库SpaceAST")
    set_description("专注于航天仿真领域的算法库，提供了轨道力学、姿态、轨迹规划等核心算法的现代C++实现")
    set_copyright("版权所有 (C) 2025-present, spaceAST项目.")
    set_author("axel <space_ast@163.com>")
    set_licensefile("LICENSE")
    set_formats("nsis", "zip", "targz", "srczip", "srctargz", "runself", "rpm", "srpm", "deb")
    add_targets("ascript")
    add_installfiles("(examples/**)|*.lua")
    add_installfiles("scripts/xpack/port.lua", {filename = "xmake.lua"})
    on_load(function(package)
        import("core.project.project")
        local plat = package:plat()
        local arch = package:arch()
        package:set("basename", "SpaceAST-v$(version)-" .. plat .. "-" .. arch)
        -- 添加所有工程目标
        for targetname, _ in pairs(project.targets()) do
            if targetname:startswith("Ast") and not targetname:startswith("AstGfx") then
                package:add("targets", targetname)
            end
        end
        -- windows下添加调试库
        if plat == "windows" then
            local debug_dir = "build/windows/" .. arch .. "/debug/"
            package:add("installfiles", debug_dir .. "Ast*D.lib", {prefixdir = "lib"})
            package:add("installfiles", debug_dir .. "Ast*D.dll", {prefixdir = "bin"})
            -- package:add("installfiles", debug_dir .. "Ast*D.pdb", {prefixdir = "bin"})
        end
    end)
   
    
