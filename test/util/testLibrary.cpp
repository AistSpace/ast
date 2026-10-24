///
/// @file      testLibrary.cpp
/// @brief     Library 类单元测试
/// @details   测试 Library 类的加载、卸载与符号解析
/// @author    axel
/// @date      2026-07-30
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。

#include "ast/Library.hpp"
#include "ast/LibraryLoader.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

// 测试默认构造
TEST(Library, DefaultConstructor)
{
    Library lib;
    EXPECT_FALSE(lib.isLoaded());
}

// 测试带文件名构造（构造即加载）
TEST(Library, ConstructorWithFileName)
{
#ifdef _WIN32
    Library lib("kernel32");
#else
    Library lib("c.so.6");
#endif
    EXPECT_TRUE(lib.isLoaded());
}

// 测试加载不存在的库
TEST(Library, LoadNonexistent)
{
    Library lib;
    errc_t rc = lib.load("nonexistent_library_12345");
    EXPECT_NE(rc, eNoError);
    EXPECT_FALSE(lib.isLoaded());
}

// 测试加载和卸载标准库
TEST(Library, LoadAndUnload)
{
    Library lib;
#ifdef _WIN32
    errc_t rc = lib.load("kernel32");
#else
    errc_t rc = lib.load("c.so.6");
#endif
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(lib.isLoaded());

    rc = lib.unload();
    EXPECT_EQ(rc, eNoError);
    EXPECT_FALSE(lib.isLoaded());
}

// 测试重复卸载
TEST(Library, UnloadTwice)
{
    Library lib;
    // 未加载时卸载应返回错误
    errc_t rc = lib.unload();
    EXPECT_NE(rc, eNoError);
}

// 测试符号解析
TEST(Library, Resolve)
{
#ifdef _WIN32
    Library lib("kernel32");
    auto proc = (void*(*)())lib.resolve("GetLastError");
#else
    Library lib("c.so.6");
    auto proc = lib.resolve("printf");
#endif
    EXPECT_NE(proc, nullptr);
}

// 测试未加载时 resolve 不会自动加载
TEST(Library, ResolveWithoutLoad)
{
    Library lib;
    void* proc = lib.resolve("printf");
    EXPECT_EQ(proc, nullptr);
    EXPECT_FALSE(lib.isLoaded());
}

// 测试解析不存在的符号
TEST(Library, ResolveNonexistentSymbol)
{
#ifdef _WIN32
    Library lib("kernel32");
#else
    Library lib("c.so.6");
#endif
    void* proc = lib.resolve("nonexistent_symbol_xyz");
    EXPECT_EQ(proc, nullptr);
}

// 测试 resolve 空指针
TEST(Library, ResolveNullSymbol)
{
    Library lib;
    void* proc = lib.resolve(nullptr);
    EXPECT_EQ(proc, nullptr);
}

// 测试 IsLibrary 静态方法
TEST(Library, IsLibrary)
{
#ifdef _WIN32
    EXPECT_TRUE(Library::IsLibrary("test.dll"));
#elif defined(__linux__)
    EXPECT_TRUE(Library::IsLibrary("test.so"));
#elif defined(__macos__)
    EXPECT_TRUE(Library::IsLibrary("test.dylib"));
#endif
    EXPECT_FALSE(Library::IsLibrary("test.txt"));
    EXPECT_FALSE(Library::IsLibrary("test"));
    EXPECT_FALSE(Library::IsLibrary(""));
}

// 测试 Resolve 静态方法
TEST(Library, StaticResolve)
{
#ifdef _WIN32
    auto proc = (void*(*)())Library::Resolve("kernel32", "GetLastError");
#else
    auto proc = Library::Resolve("c.so.6", "printf");
#endif
    EXPECT_NE(proc, nullptr);
}

// 测试静态 Resolve 空参数
TEST(Library, StaticResolveNullParams)
{
    void* proc = Library::Resolve("", "func");
    EXPECT_EQ(proc, nullptr);

    proc = Library::Resolve("lib", nullptr);
    EXPECT_EQ(proc, nullptr);
}

// 测试重复加载会替换旧库
TEST(Library, Reload)
{
#ifdef _WIN32
    Library lib("kernel32");
    errc_t rc = lib.load("kernel32");
#else
    Library lib("c.so.6");
    errc_t rc = lib.load("c.so.6");
#endif
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(lib.isLoaded());
}

// 测试获取动态库文件路径
TEST(Library, FilePath)
{
    Library lib;
    EXPECT_TRUE(lib.filePath().empty());  // 未加载时为空

#ifdef _WIN32
    lib.load("kernel32");
#else
    lib.load("c.so.6");
#endif
    EXPECT_TRUE(lib.isLoaded());
    EXPECT_FALSE(lib.filePath().empty());
}

// 测试析构函数自动卸载
TEST(Library, DestructorUnloads)
{
    // 用指针控制生命周期
#ifdef _WIN32
    auto* lib = new Library("kernel32");
#else
    auto* lib = new Library("c.so.6");
#endif
    EXPECT_TRUE(lib->isLoaded());

    delete lib;  // 析构应自动卸载，不崩溃
    // 如果到达这里没有异常，说明析构正常
}

GTEST_MAIN()
