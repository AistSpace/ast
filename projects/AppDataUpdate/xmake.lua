target("AppDataUpdate")
    add_rules("ast.qt.widgetapp")
    add_files("**.cpp")
    if is_plat("windows") and is_arch("x86") then
        -- 嵌入 AppDataUpdate.manifest（声明 asInvoker）
        -- 文件名含 "Update" 且无清单的 32 位 exe 会被 Windows 安装程序检测按更新程序处理并自动弹 UAC 提权
        -- 明确声明 requestedExecutionLevel 保持与启动者同权限
        add_files("AppDataUpdate.rc")
    end
    add_deps("AstUiDataUpdate", "AstCore")
    if not has_package("qt") then
        set_enabled(false)
    end