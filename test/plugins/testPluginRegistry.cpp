///
/// @file      testPluginRegistry.cpp
/// @brief     插件注册表基础行为测试
/// @details   验证扫描/查找接口在无插件情况下的边界行为
/// @author    axel
/// @date      2026-10-14
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "ast/Test.hpp"
#include "ast/PluginRegistry.hpp"
#include "ast/FileSystem.hpp"
#include "ast/Logger.hpp"
#include <string>
#include <vector>

AST_USING_NAMESPACE

TEST(PluginRegistry, ScanInvalidPath)
{
    // 空路径
    EXPECT_EQ(PluginRegistry::Instance()->scanDirectory(StringView()), eErrorNullInput);

    // 不存在的目录
    const std::string missing =
        (fs::temp_directory_path() / "ast_plugin_missing_dir_9f2a").string();
    EXPECT_EQ(PluginRegistry::Instance()->scanDirectory(missing), eErrorInvalidFile);
}

TEST(PluginRegistry, EmptyDirScanAndMissLookup)
{
    PluginRegistry* registry = PluginRegistry::Instance();

    const fs::path dir = fs::temp_directory_path() / "ast_plugin_empty_dir_9f2a";
    fs::remove_all(dir);
    if (!fs::create_directory(dir))
    {
        aWarning(_("无法创建临时目录，跳过本用例"));
        return;
    }

    // 空目录：扫描成功，但不产生任何条目
    EXPECT_EQ(registry->scanDirectory(dir.string()), eNoError);
    EXPECT_EQ(registry->entryCount(), static_cast<size_t>(0));

    // 未命中接口：keys 为空、find 返回 nullptr
    std::vector<std::string> keys;
    registry->keys("com.example.NoSuchInterface", keys);
    EXPECT_TRUE(keys.empty());
    EXPECT_TRUE(registry->find("com.example.NoSuchInterface", "anything") == nullptr);

    // 重复扫描同一目录是幂等的
    EXPECT_EQ(registry->scanDirectory(dir.string()), eNoError);
    EXPECT_EQ(registry->entryCount(), static_cast<size_t>(0));

    fs::remove_all(dir);
}

GTEST_MAIN()
