-- 大气模型插件：编译为独立动态库，运行时由宿主扫描加载（不参与链接）
target("AstPluginAtmosphere")
    set_kind("shared")
    add_files("**.cpp")
    add_deps("AstCore")
