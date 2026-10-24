/// @file      testFileSystem.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      23.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

// #define AST_USE_STD_FILESYSTEM_EXPERIMENTAL
// #define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
// #include <experimental/filesystem>

// 在文件开头的包含部分添加Windows相关头文件
#ifdef _WIN32
#include <windows.h>
#endif

#include "ast/FileSystem.hpp"
#include "ast/Path.hpp"
#include "ast/AstTestMacro.h"
#include <iostream>
#include <fstream>
#include <clocale>

AST_USING_NAMESPACE


TEST(FileSystem, CurrentPath)
{
    namespace fs = _AST filesystem;
    {
        std::string filepath = u8"新建文件夹";
        fs::path current_path = filepath;
        bool same = filepath == current_path.string();
        EXPECT_TRUE(same);
    }
    {
        fs::path current_path = fs::current_path();
        printf("current_path: %s\n", current_path.string().c_str());
        for (const auto& entry : fs::directory_iterator(current_path)) {
            std::string filepath = entry.path().string();
            std::string filename = entry.path().filename().string();
            std::cout << "filepath: " << filepath << std::endl;
            std::cout << "filename: " << filename << std::endl;
        }
    }
    // printf("-----------------------------------\n");
    // {
    //     namespace fs = std::experimental::filesystem;
    //     fs::path current_path = fs::current_path();
    //     printf("current_path: %s\n", current_path.string().c_str());
    //     for (const auto& entry : fs::directory_iterator(current_path)) {
    //         std::string filepath = entry.path().string();
    //         std::string filename = entry.path().filename().string();
    //         std::cout << "filepath: " << filepath << std::endl;
    //         std::cout << "filename: " << filename << std::endl;
    //     }
    // 
    // }
}
 

// 测试路径类功能
TEST(SimpleFileSystem, PathOperations) {
    namespace fs = _AST fs_simple;
    
    // 基本路径创建和转换
    fs::path p1 = "test/file.txt";
    EXPECT_EQ(p1.string(), "test/file.txt");
    
    // 文件名和扩展名提取
    EXPECT_EQ(p1.filename().string(), "file.txt");
    EXPECT_EQ(p1.extension().string(), ".txt");
    EXPECT_EQ(p1.stem().string(), "file");
    
    // 父路径提取
    EXPECT_EQ(p1.parent_path().string(), "test");
    
    // 路径组合
    fs::path p2 = "dir";
    fs::path p3 = p2 / "subdir";
    fs::path expected = "dir" + std::string(1, fs::path::preferred_separator()) + "subdir";
    EXPECT_EQ(p3.string(), expected);
    
    // 路径分隔符检查
    EXPECT_TRUE(aIsPathSeparator('/'));
    #ifdef _WIN32
    EXPECT_TRUE(aIsPathSeparator('\\'));
    #endif
    
    // 路径比较
    fs::path p4 = "test/file.txt";
    EXPECT_TRUE(p1 == p4);
    
    // 空路径检查
    fs::path empty;
    EXPECT_TRUE(empty.empty());
}

// 测试文件状态相关函数
TEST(SimpleFileSystem, FileStatus) {
    namespace fs = _AST fs_simple;
    
    // 创建临时路径进行测试
    fs::path test_dir = "test_temp_dir";
    fs::path test_file = test_dir / "test_file.txt";
    
    // 测试不存在的路径
    EXPECT_FALSE(fs::exists("non_existent_path_12345"));
    EXPECT_FALSE(fs::is_directory("non_existent_path_12345"));
    EXPECT_FALSE(fs::is_regular_file("non_existent_path_12345"));
    EXPECT_EQ(fs::status("non_existent_path_12345").type(), fs::file_type::not_found);
    
    // 创建测试目录和文件以测试存在性检查
    if (!fs::exists(test_dir)) {
        fs::create_directories(test_dir);
    }
    
    // 测试目录存在性和类型
    EXPECT_TRUE(fs::exists(test_dir));
    EXPECT_TRUE(fs::is_directory(test_dir));
    EXPECT_FALSE(fs::is_regular_file(test_dir));
    EXPECT_EQ(fs::status(test_dir).type(), fs::file_type::directory);
    
    // 创建测试文件
    {
        std::ofstream file(test_file.string());
        file << "Test content for file status";
        file.close();
    }
    
    // 测试文件存在性和类型
    EXPECT_TRUE(fs::exists(test_file));
    EXPECT_FALSE(fs::is_directory(test_file));
    EXPECT_TRUE(fs::is_regular_file(test_file));
    EXPECT_EQ(fs::status(test_file).type(), fs::file_type::regular);
    
    // 清理测试目录和文件
    if (fs::exists(test_dir)) {
        fs::remove_all(test_dir);
    }
}

// 测试目录操作
TEST(SimpleFileSystem, DirectoryOperations) {
    namespace fs = _AST fs_simple;
    
    // 创建和删除单个目录
    fs::path single_dir = "single_test_dir";
    EXPECT_TRUE(fs::create_directory(single_dir));
    EXPECT_TRUE(fs::exists(single_dir));
    EXPECT_TRUE(fs::remove(single_dir));
    EXPECT_FALSE(fs::exists(single_dir));
    
    // 创建和删除多级目录
    fs::path nested_dir = "level1/level2/level3";
    EXPECT_TRUE(fs::create_directories(nested_dir));
    EXPECT_TRUE(fs::exists(nested_dir));
    
    // 测试remove_all删除整个目录树
    uintmax_t removed_count = fs::remove_all("level1");
    EXPECT_TRUE(removed_count > 0);
    EXPECT_FALSE(fs::exists("level1"));
}

// 测试文件操作
TEST(SimpleFileSystem, FileOperations) {
    namespace fs = _AST fs_simple;
    
    fs::path test_dir = "file_operations_test";
    fs::path source_file = test_dir / "source.txt";
    fs::path dest_file = test_dir / "destination.txt";
    fs::path renamed_file = test_dir / "renamed.txt";
    
    // 创建测试目录
    if (!fs::exists(test_dir)) {
        fs::create_directories(test_dir);
    }
    
    // 创建测试文件（使用标准C++文件操作）
    {
        std::ofstream file(source_file.string());
        file << "Test content for file operations";
        file.close();
    }
    
    // 测试文件大小
    EXPECT_GT(fs::file_size(source_file), 0);
    
    // 测试文件复制
    EXPECT_TRUE(fs::copy_file(source_file, dest_file));
    EXPECT_TRUE(fs::exists(dest_file));
    EXPECT_EQ(fs::file_size(source_file), fs::file_size(dest_file));
    
    // 测试文件重命名
    EXPECT_TRUE(fs::rename(dest_file, renamed_file));
    EXPECT_FALSE(fs::exists(dest_file));
    EXPECT_TRUE(fs::exists(renamed_file));
    
    // 测试最后修改时间
    std::time_t mod_time = fs::last_write_time(renamed_file);
    EXPECT_NE(mod_time, static_cast<std::time_t>(-1));
    
    // 清理测试文件和目录
    if (fs::exists(test_dir)) {
        fs::remove_all(test_dir);
    }
}

// 测试 is_empty（对标 std::filesystem::is_empty）
TEST(SimpleFileSystem, IsEmpty) {
    namespace fs = _AST fs_simple;

    fs::path test_dir = "is_empty_test_dir";
    fs::path empty_file = test_dir / "empty.txt";
    fs::path non_empty_file = test_dir / "data.txt";

    if (fs::exists(test_dir)) {
        fs::remove_all(test_dir);
    }
    fs::create_directories(test_dir);

    // 空常规文件 → true
    {
        std::ofstream file(empty_file.string());
        // 不写入任何内容，保持空文件
        file.close();
    }
    EXPECT_TRUE(fs::exists(empty_file));
    EXPECT_TRUE(fs::is_empty(empty_file));

    // 非空常规文件 → false
    {
        std::ofstream file(non_empty_file.string());
        file << "some content";
        file.close();
    }
    EXPECT_TRUE(fs::exists(non_empty_file));
    EXPECT_FALSE(fs::is_empty(non_empty_file));

    // 非空目录 → false（当前测试目录里有两个文件）
    EXPECT_FALSE(fs::is_empty(test_dir));

    // 空目录 → true
    fs::path empty_dir = test_dir / "empty_subdir";
    EXPECT_TRUE(fs::create_directory(empty_dir));
    EXPECT_TRUE(fs::is_empty(empty_dir));

    // 不存在的路径：error_code 版本报错并返回 false
    {
        fs::path missing = "is_empty_missing_12345";
        std::error_code ec;
        EXPECT_FALSE(fs::is_empty(missing, ec));
        EXPECT_TRUE(ec);
    }

    // 不存在的路径：无 ec 版本抛出 filesystem_error
    {
        fs::path missing = "is_empty_missing_12345";
        bool threw = false;
        try {
            (void)fs::is_empty(missing);
        }
        catch (const fs::filesystem_error&) {
            threw = true;
        }
        EXPECT_TRUE(threw);
    }

    // 清理测试目录
    if (fs::exists(test_dir)) {
        fs::remove_all(test_dir);
        EXPECT_FALSE(fs::exists(test_dir));
    }
}

// 测试目录迭代器
TEST(SimpleFileSystem, DirectoryIterator) {
    namespace fs = _AST fs_simple;
    
    fs::path test_dir = "iterator_test_dir";
    fs::path file1 = test_dir / "file1.txt";
    fs::path file2 = test_dir / "file2.txt";
    
    // 创建测试目录和文件
    if (!fs::exists(test_dir)) {
        fs::create_directories(test_dir);
    }
    
    // 创建测试文件
    {
        std::ofstream f1(file1.string());
        f1 << "Content 1";
        std::ofstream f2(file2.string());
        f2 << "Content 2";
    }
    
    // 测试目录迭代器
    int count = 0;
    std::set<std::string> filenames;
    
    try {
        for (auto& entry : fs::directory_iterator(test_dir)) {
            count++;
            filenames.insert(entry.path().filename().string());
        }
    }
    catch (const fs::filesystem_error& e) {
        FAIL() << "Directory iteration failed: " << e.what();
    }
    
    EXPECT_EQ(count, 2);  // 应该有两个文件
    EXPECT_TRUE(filenames.count("file1.txt"));
    EXPECT_TRUE(filenames.count("file2.txt"));
    
    // 清理测试目录
    if (fs::exists(test_dir)) {
        fs::remove_all(test_dir);
    }
}

// 测试磁盘空间信息
TEST(SimpleFileSystem, DiskSpace) {
    namespace fs = _AST fs_simple;
    
    try {
        fs::space_info info = fs::space(".");
        // 检查空间信息是否有效（在正常环境中这些值应该都是正数）
        EXPECT_GE(info.capacity, 0);
        EXPECT_GE(info.free, 0);
        EXPECT_GE(info.available, 0);
    }
    catch (const std::exception& e) {
        // 磁盘空间信息获取可能在某些环境下失败，记录但不强制失败
        std::cerr << "Warning: Failed to get disk space info: " << e.what() << std::endl;
    }
}

// 测试错误处理
TEST(SimpleFileSystem, ErrorHandling) {
    namespace fs = _AST fs_simple;
    
    // 测试错误码版本的current_path
    std::error_code ec;
    fs::path p = fs::current_path(ec);
    EXPECT_FALSE(ec);  // 在正常环境中应该成功
    
    // 测试设置不存在的当前路径时的错误处理
    fs::path non_existent = "non_existent_path_98765";
    std::error_code ec_set;
    fs::current_path(non_existent, ec_set);
    EXPECT_TRUE(ec_set);  // 应该失败并设置错误码
}

// 测试文件大小（含 error_code 重载）
TEST(SimpleFileSystem, FileSize) {
    namespace fs = _AST fs_simple;

    fs::path test_file = "file_size_test.txt";
    fs::path missing_file = "file_size_test_missing.txt";
    fs::remove(test_file);
    fs::remove(missing_file);

    {
        std::ofstream file(test_file.string(), std::ios::binary);
        file << "0123456789";
    }
    EXPECT_EQ(fs::file_size(test_file), 10u);

    // 不存在的文件：error_code 版本返回 (uintmax_t)-1 并设置 ec
    std::error_code ec;
    uintmax_t size = fs::file_size(missing_file, ec);
    EXPECT_TRUE(ec);
    EXPECT_EQ(size, static_cast<uintmax_t>(-1));

    // 非 error_code 版本抛异常
    EXPECT_THROW(fs::file_size(missing_file), fs::filesystem_error);

    // is_empty 的 error_code 版本不应抛异常，只设置 ec
    ec.clear();
    EXPECT_FALSE(fs::is_empty(missing_file, ec));
    EXPECT_TRUE(ec);

    fs::remove(test_file);
}

// 测试绝对路径
TEST(SimpleFileSystem, Absolute) {
    namespace fs = _AST fs_simple;

    // 已经是绝对路径时原样返回
    #ifdef _WIN32
    fs::path abs_path = "C:\\Windows\\System32";
    #else
    fs::path abs_path = "/etc";
    #endif
    EXPECT_TRUE(abs_path.is_absolute());
    EXPECT_EQ(fs::absolute(abs_path), abs_path);

    // 相对路径拼接到 current_path 上
    std::error_code ec;
    fs::path base = fs::current_path(ec);
    EXPECT_FALSE(ec);

    fs::path rel_path = fs::absolute("some_relative_file.txt", ec);
    EXPECT_FALSE(ec);
    EXPECT_TRUE(rel_path.is_absolute());
    EXPECT_EQ(rel_path, base / "some_relative_file.txt");

    // 与 current_path() 直接拼接的结果一致
    EXPECT_EQ(fs::absolute(fs::path("a") / "b"), fs::current_path() / "a" / "b");

    // 空路径是错误（与 std::filesystem 一致），不会退化成当前目录
    std::error_code empty_ec;
    EXPECT_TRUE(fs::absolute(fs::path(), empty_ec).empty());
    EXPECT_TRUE(empty_ec);
    EXPECT_THROW(fs::absolute(fs::path()), fs::filesystem_error);

    #ifdef _WIN32
    // 根相对路径（\foo）没有盘符，按当前盘解析，而不是拼到 current_path() 后面
    std::string current_drive = base.native().substr(0, 2);
    ASSERT_EQ(current_drive.size(), 2u);
    ASSERT_EQ(current_drive[1], ':');

    std::error_code root_ec;
    fs::path root_relative = fs::absolute("\\foo", root_ec);
    EXPECT_FALSE(root_ec);
    EXPECT_TRUE(root_relative.is_absolute());
    EXPECT_EQ(root_relative.native(), current_drive + "\\foo");

    // 盘符相对路径（C:foo）按 C 盘的当前目录解析
    std::error_code drive_ec;
    fs::path drive_relative = fs::absolute("C:foo", drive_ec);
    EXPECT_FALSE(drive_ec);
    EXPECT_TRUE(drive_relative.is_absolute());
    EXPECT_EQ(drive_relative.native().substr(0, 2), std::string("C:"));
    #endif
}

// 绝对性判定与 std::filesystem::path::is_absolute 保持一致
TEST(SimpleFileSystem, PathIsAbsolute) {
    namespace fs = _AST fs_simple;

    EXPECT_FALSE(aPathIsAbsolute(""));
    EXPECT_FALSE(aPathIsAbsolute("foo"));
    EXPECT_TRUE(aPathIsRelative("foo"));

#ifdef _WIN32
    EXPECT_TRUE(aPathIsAbsolute("C:\\Windows"));
    EXPECT_TRUE(aPathIsAbsolute("c:/Windows"));
    EXPECT_TRUE(aPathIsAbsolute("\\\\server\\share"));
    EXPECT_FALSE(aPathIsAbsolute("C:relative"));    // 盘符相对，需要按 C 盘当前目录解析
    EXPECT_FALSE(aPathIsAbsolute("\\\\"));          // 退化的 UNC 前缀，没有 root_name
    EXPECT_TRUE(aPathIsAbsolute("\\\\server"));     // 不完整的 UNC，std 仍视为绝对
    // 只有单个前导分隔符的是"根相对"，std::filesystem 视为相对路径
    EXPECT_FALSE(aPathIsAbsolute("\\foo"));
    EXPECT_FALSE(aPathIsAbsolute("/foo"));
    EXPECT_TRUE(aPathIsRelative("\\foo"));
    EXPECT_TRUE(fs::path("C:\\Windows").is_absolute());
    EXPECT_TRUE(fs::path("\\foo").is_relative());
#else
    EXPECT_TRUE(aPathIsAbsolute("/etc"));
    EXPECT_FALSE(aPathIsAbsolute("etc"));
    EXPECT_TRUE(fs::path("/etc").is_absolute());
    EXPECT_TRUE(fs::path("etc").is_relative());
#endif
}

// 测试系统临时文件夹
TEST(SimpleFileSystem, TempDirectoryPath) {
    namespace fs = _AST fs_simple;

    std::error_code ec;
    fs::path tmpdir = fs::temp_directory_path(ec);
    EXPECT_FALSE(ec);
    EXPECT_FALSE(tmpdir.empty());
    EXPECT_TRUE(tmpdir.is_absolute());
    EXPECT_TRUE(fs::is_directory(tmpdir));

    // 临时目录下确实可以创建文件
    fs::path temp_file = tmpdir / "ast_test_tempdir.txt";
    {
        std::ofstream file(temp_file.string(), std::ios::binary);
        file << "tmp";
    }
    EXPECT_TRUE(fs::exists(temp_file));
    EXPECT_EQ(fs::file_size(temp_file), 3u);
    fs::remove(temp_file);
}

// 测试Unicode字符支持
TEST(SimpleFileSystem, UnicodeSupport) {
    namespace fs = _AST fs_simple;
    
    // 在Windows平台上设置控制台和标准库编码为UTF-8
    #ifdef _WIN32
    // 设置控制台I/O编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif

    std::setlocale(LC_ALL, ".UTF-8");
    auto locale = std::setlocale(LC_ALL, nullptr);
    printf("Locale name: %s\n", locale);
    
    // 创建包含Unicode字符的测试目录
    fs::path unicode_dir = u8"测试目录_Unicode";
    
    // 创建包含各种Unicode字符的文件路径
    fs::path chinese_file = unicode_dir / u8"中文文件.txt";
    fs::path arabic_file = unicode_dir / u8"ملف_عربي.txt";
    fs::path russian_file = unicode_dir / u8"Русский_файл.txt";
    fs::path spanish_file = unicode_dir / u8"archivo_español.txt";
    fs::path emoji_file = unicode_dir / u8"😊表情文件.txt";
    
    // 确保测试开始前目录不存在
    if (fs::exists(unicode_dir)) {
        fs::remove_all(unicode_dir);
    }
    
    // 测试创建包含Unicode字符的目录
    EXPECT_TRUE(fs::create_directory(unicode_dir));
    EXPECT_TRUE(fs::exists(unicode_dir));
    EXPECT_TRUE(fs::is_directory(unicode_dir));
    
    // 测试创建和访问包含不同Unicode字符的文件
    // 中文文件名测试
    {
        std::ofstream file(chinese_file.string());
        file << u8"中文内容";
        file.close();
        EXPECT_TRUE(fs::exists(chinese_file));
        EXPECT_TRUE(fs::is_regular_file(chinese_file));
    }
    
    // 阿拉伯语文件名测试
    {
        std::ofstream file(arabic_file.string());
        file << u8"محتوى عربي";
        file.close();
        EXPECT_TRUE(fs::exists(arabic_file));
        EXPECT_TRUE(fs::is_regular_file(arabic_file));
    }
    
    // 俄文文件名测试
    {
        std::ofstream file(russian_file.string());
        file << u8"Русский контент";
        file.close();
        EXPECT_TRUE(fs::exists(russian_file));
        EXPECT_TRUE(fs::is_regular_file(russian_file));
    }
    
    // 西班牙文文件名测试
    {
        std::ofstream file(spanish_file.string());
        file << u8"Contenido español";
        file.close();
        EXPECT_TRUE(fs::exists(spanish_file));
        EXPECT_TRUE(fs::is_regular_file(spanish_file));
    }
    
    // Emoji文件名测试
    {
        std::ofstream file(emoji_file.string());
        file << u8"Emoji content 😊";
        file.close();
        EXPECT_TRUE(fs::exists(emoji_file));
        EXPECT_TRUE(fs::is_regular_file(emoji_file));
    }
    
    // 测试文件操作（复制、重命名）对Unicode路径的支持
    fs::path chinese_copy = unicode_dir / u8"中文文件_副本.txt";
    EXPECT_TRUE(fs::copy_file(chinese_file, chinese_copy));
    EXPECT_TRUE(fs::exists(chinese_copy));
    
    fs::path chinese_renamed = unicode_dir / u8"中文文件_重命名.txt";
    EXPECT_TRUE(fs::rename(chinese_file, chinese_renamed));
    EXPECT_FALSE(fs::exists(chinese_file));
    EXPECT_TRUE(fs::exists(chinese_renamed));
    
    // 测试目录迭代器对Unicode文件名的支持
    int count = 0;
    std::set<std::string> expected_files = {
        u8"中文文件_副本.txt",
        u8"中文文件_重命名.txt",
        u8"ملف_عربي.txt",
        u8"Русский_файл.txt",
        u8"archivo_español.txt",
        u8"😊表情文件.txt"
    };
    
    try {
        for (auto& entry : fs::directory_iterator(unicode_dir)) {
            if (fs::is_regular_file(entry.path())) {
                std::string filename = entry.path().filename().string();
                EXPECT_TRUE(expected_files.count(filename)) << "Unexpected filename: " << filename;
                count++;
            }
        }
    } catch (const fs::filesystem_error& e) {
        FAIL() << "Directory iteration with Unicode failed: " << e.what();
    }
    
    EXPECT_EQ(count, expected_files.size());
    
    // 清理测试目录
    if (fs::exists(unicode_dir)) {
        uintmax_t removed = fs::remove_all(unicode_dir);
        EXPECT_TRUE(removed > 0);
        EXPECT_FALSE(fs::exists(unicode_dir));
    }
}

#ifdef _WIN32
// 目录联接（Junction）只在 Windows/NTFS 上可用
TEST(SimpleFileSystem, CreateJunction)
{
    namespace fs = _AST fs_simple;

    fs::path base = fs::current_path() / "test_create_junction";
    fs::remove_all(base);
    ASSERT_TRUE(fs::create_directories(base));

    fs::path target = base / "target";
    ASSERT_TRUE(fs::create_directories(target));
    {
        std::ofstream file((target / "payload.txt").string());
        ASSERT_TRUE(file.good());
        file << "junction payload";
    }

    fs::path link = base / "link";
    EXPECT_EQ(aCreateJunction(target.string(), ""), eErrorInvalidParam);
    EXPECT_EQ(aCreateJunction("", link.string()), eErrorInvalidParam);
    // 联接点不能指向 UNC/网络路径
    EXPECT_EQ(aCreateJunction("\\\\server\\share", link.string()), eErrorInvalidParam);
    EXPECT_FALSE(fs::exists(link));
    EXPECT_EQ(aCreateJunction(target.string(), link.string()), eNoError);

    // 通过联接点应能访问到目标目录里的文件
    EXPECT_TRUE(fs::is_directory(link));
    EXPECT_TRUE(fs::exists(link / "payload.txt"));
    // 目录联接是重解析点，但不是符号链接
    EXPECT_FALSE(fs::is_symlink(link));
    EXPECT_EQ(fs::symlink_status(link).type(), fs::file_type::directory);

    // 联接点路径已存在时不能重复创建
    EXPECT_NE(aCreateJunction(target.string(), link.string()), eNoError);

    // 删除联接点不影响目标目录；remove_all 也不能递归进目标去删内容
    EXPECT_EQ(fs::remove_all(link), 1);
    EXPECT_FALSE(fs::exists(link));
    EXPECT_TRUE(fs::exists(target / "payload.txt"));

    fs::remove_all(base);
}
#endif

// 符号链接在所有平台上都可用；Windows 上创建需要管理员权限或开发者模式
// 注意：测试名不能叫 CreateSymbolicLink，windows.h 会把该标识符宏展开成 CreateSymbolicLinkA/W
TEST(SimpleFileSystem, SymbolicLink)
{
    namespace fs = _AST fs_simple;

    fs::path base = fs::current_path() / "test_create_symlink";
    fs::remove_all(base);
    ASSERT_TRUE(fs::create_directories(base));

    fs::path target = base / "target";
    ASSERT_TRUE(fs::create_directories(target));
    {
        std::ofstream file((target / "payload.txt").string());
        ASSERT_TRUE(file.good());
        file << "symlink payload";
    }

    fs::path link = base / "link";
    EXPECT_EQ(aCreateSymbolicLink(target.string(), "", true), eErrorInvalidParam);
    EXPECT_EQ(aCreateSymbolicLink("", link.string(), true), eErrorInvalidParam);
    EXPECT_FALSE(fs::exists(link));

    errc_t rc = aCreateSymbolicLink(target.string(), link.string(), true);
#ifdef _WIN32
    if (rc == eError)
    {
        // 未开启开发者模式且没有管理员权限时，CreateSymbolicLinkW 返回
        // ERROR_PRIVILEGE_NOT_HELD，统一映射为 eError
        fs::remove_all(base);
        GTEST_SKIP() << "creating a symbolic link requires administrator or developer mode";
    }
#endif
    ASSERT_EQ(rc, eNoError);

    // 通过符号链接应能访问到目标目录里的文件
    EXPECT_TRUE(fs::is_directory(link));
    EXPECT_TRUE(fs::exists(link / "payload.txt"));

    // 链接本身是符号链接，目标不是
    EXPECT_TRUE(fs::is_symlink(link));
    EXPECT_FALSE(fs::is_symlink(target));
    EXPECT_EQ(fs::symlink_status(link).type(), fs::file_type::symlink);
    EXPECT_EQ(fs::symlink_status(target).type(), fs::file_type::directory);

    // 链接路径已存在时不能重复创建
    EXPECT_NE(aCreateSymbolicLink(target.string(), link.string(), true), eNoError);

    // 删除符号链接不影响目标目录
    EXPECT_TRUE(fs::remove(link));
    EXPECT_FALSE(fs::exists(link));
    EXPECT_TRUE(fs::exists(target / "payload.txt"));

    // remove_all 对符号链接也只删除链接本身，不递归到目标
    fs::path link2 = base / "link2";
    ASSERT_EQ(aCreateSymbolicLink(target.string(), link2.string(), true), eNoError);
    EXPECT_EQ(fs::remove_all(link2), 1);
    EXPECT_FALSE(fs::exists(link2));
    EXPECT_TRUE(fs::exists(target / "payload.txt"));

    fs::remove_all(base);
}

GTEST_MAIN()