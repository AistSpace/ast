add_rules("mode.release", "mode.debug")         -- release 模式和 debug 模式
set_targetdir("bin")                            -- 输出目录
add_includedirs("include")                      -- 头文件目录
add_linkdirs("lib")                             -- 链接库目录
add_defines("AST_NO_CSPICE", "AST_NO_MATPLOT")  -- 宏定义：禁用 CSPICE 和 matplot

-- 链接库
if is_plat("windows") and is_mode("debug") then
    add_links("AstCoreD", "AstUtilD", "AstMathD")
else
    add_links("AstCore", "AstUtil", "AstMath")
end


-- 遍历添加示例工程
local files = os.files("examples/**/*.c*")
for _, file in ipairs(files) do
    local targetname = file:gsub("[\\/]", "_"):gsub("%.[^.]*$", "")
    local basename = path.basename(file)
    target(basename)
        add_files(file)
        set_kind("binary")
        add_tests("example")

        if is_plat("windows") then
            add_cxflags("/utf-8")
        end
end

