-- JavaScript 脚本执行器插件：依赖可选的 Duktape，未启用该包时不构建
if has_package("duktape") then
    -- 定义该宏表示具有 Duktape 引擎
    -- 否则只定义返回 eErrorUnsupported 的桩实现
    add_defines("AST_WITH_DUKTAPE")

    target("AstPluginJavaScript")
        set_kind("shared")
        add_files("**.cpp")
        add_deps("AstScript")
        add_packages("duktape")
        add_defines("AST_BUILD_PLUGIN_JAVASCRIPT")
end
