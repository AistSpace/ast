-- 超长数据目录测试的辅助工具
target("PrepareLongPathData")
    add_files("*.cxx")
    set_kind("binary")
    add_deps("AstUtil", "AstCore", "AstTest")
