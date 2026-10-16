#include "ast/AstTestMacro.h"
#include "ast/IO.hpp"
#include "ast/RunTime.hpp"
#include <clocale>

#ifdef _WIN32
#include <Windows.h>
#endif

AST_USING_NAMESPACE

void testPrintf()
{
    std::printf("----------------------\n");
    std::printf("std::printf:\n");
    std::wprintf(L"%s", L"😊😀\n");
    std::printf("%s", u8"😊😀\n");
    std::printf("%s", u8"中文utf8\n");
    std::fprintf(stdout, "%s", u8"中文utf8\n");
    std::printf("%s", "中文gbk\n");
    std::printf("%s", u8"محتوى عربي\n");

    std::printf("----------------------\n");
    std::printf("posix printf:\n");
    posix::printf("%s", u8"😊😀 ast\n");
    posix::printf("%s", u8"中文utf8 ast\n");
    posix::fprintf(stdout, "%s", u8"中文utf8 ast\n");
    posix::printf("%s", u8"محتوى عربي\n");
    std::printf("----------------------\n");
}

TEST(IO, printf)
{
    testPrintf();
    for(auto locale: {".UTF-8", "", "C"})
    {
        setlocale(LC_ALL, locale);
        auto currentLocale = std::setlocale(LC_ALL, nullptr);
        std::printf("Locale name: %s\n", currentLocale);
        testPrintf();
    }

    #ifdef _WIN32
    {
        // 在Windows平台上设置控制台和标准库编码为UTF-8
        // 设置控制台I/O编码为UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        testPrintf();
    }
     #endif

}

TEST(IO, getFilePath)
{
    {
        std::string datadir = aTestGetConfigValue("LSK_FILE").toString();
        FILE* file = posix::fopen(datadir.c_str(), "r");
        EXPECT_TRUE(file != nullptr);

        std::string filepath;
        auto rc = aGetFilePath(file, filepath);
        std::printf("filepath: %s\n", filepath.c_str());
        EXPECT_EQ(rc, eNoError);
        EXPECT_FALSE(filepath.empty());
        FILE* file2 = posix::fopen(filepath.c_str(), "r");
        EXPECT_TRUE(file2 != nullptr);
    }
    {
        std::string filepath;
        auto rc = aGetFilePath(stdin, filepath);
        std::printf("rc: %d\n", rc);
        std::printf("stdin filepath: %s\n", filepath.c_str());
    }
    {
        std::string filepath;
        auto rc = aGetFilePath(stdout, filepath);
        std::printf("rc: %d\n", rc);
        std::printf("stdout filepath: %s\n", filepath.c_str());
    }
}


GTEST_MAIN();

