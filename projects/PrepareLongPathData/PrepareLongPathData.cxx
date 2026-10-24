///
/// @file      PrepareLongPathData.cxx
/// @brief     为超长数据目录测试准备/校验深路径的 data 与 test-data
/// @details   
/// 两种模式：
///  - 默认：在临时目录下建一个超过 MAX_PATH 的深路径，
///    用目录联接（Windows）或符号链接（其它平台）把 data 与 test-data 链过去，
///    并把结果写入GITHUB_ENV（本地直接运行时只打印路径）
///  - `--check`：校验 AST_DATA_DIR / AST_TEST_DATA_DIR 指向的目录
///    确实可用。aDataDir() 在目录不可见时会静默回退到默认目录，
///    这里显式比较，避免整套测试跑在短路径上还误判为通过。
///
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/FileSystem.hpp"
#include "ast/FileStream.hpp"
#include "ast/Path.hpp"
#include "ast/Posix.hpp"
#include "ast/RunTime.hpp"
#include "ast/TestConfig.hpp"
#include "ast/IO.hpp"

#include <cstdio>
#include <cstring>
#include <string>

AST_USING_NAMESPACE

namespace {

/// 根目录至少这么长：同时超过 Windows 的 MAX_PATH(260) 和 CSPICE 的 FILSIZ(255)
const size_t kMinRootLength = 360;

/// 单级目录名长度，远小于单个路径分量的上限 255
const size_t kSegmentLength = 50;

/// 校验时用来确认数据目录可读的文件
const char* kTestDataFile = "/LeapSecond.dat";
const char* kDataFile = "/SolarSystem/Earth/EOP-All.txt";


std::string aEnvValue(const char* name)
{
    const char* value = posix::getenv(name);
    return value ? std::string(value) : std::string();
}


bool aLinkDir(const fs::path& target, const fs::path& link)
{
    // 目标必须是绝对路径：aCreateJunction 会按当前目录解析相对路径，
    // 而符号链接保存的是目标字符串、访问时按链接所在目录解析，语义不同
#ifdef _WIN32
    errc_t rc = aCreateJunction(target.string(), link.string());
#else
    errc_t rc = aCreateSymbolicLink(target.string(), link.string(), true);
#endif
    if (rc != eNoError) {
        posix::fprintf(stderr, "创建目录链接失败 '%s' -> '%s'（错误码 %d）\n",
                link.string().c_str(), target.string().c_str(), static_cast<int>(rc));
        return false;
    }
    return true;
}


bool aCanReadFile(const std::string& filepath)
{
    std::FILE* fp = posix::fopen(filepath.c_str(), "rb");
    if (!fp) {
        posix::fprintf(stderr, "无法打开文件 '%s'\n", filepath.c_str());
        return false;
    }
    char buffer[64];
    size_t count = std::fread(buffer, 1, sizeof(buffer), fp);
    std::fclose(fp);
    if (count == 0) {
        posix::fprintf(stderr, "文件为空或读取失败 '%s'\n", filepath.c_str());
        return false;
    }
    return true;
}


fs::path aMakeLongRoot()
{
    fs::path root = fs::temp_directory_path() / "ast-longpath-data";
    int index = 0;
    while (root.string().size() < kMinRootLength) {
        root /= std::string(kSegmentLength, static_cast<char>('a' + index % 26));
        ++index;
    }
    return root;
}


int aPrepare()
{
    fs::path cwd = fs::current_path();
    fs::path srcData = cwd / "data";
    fs::path srcTestData = cwd / "test-data";
    if (!fs::is_directory(srcData) || !fs::is_directory(srcTestData)) {
        posix::fprintf(stderr, "当前目录 '%s' 下缺少 data 或 test-data，请先执行 xmake cpdata\n",
                cwd.string().c_str());
        return 1;
    }

    fs::path root = aMakeLongRoot();
    fs::remove_all(root);
    std::error_code ec;
    if (!fs::create_directories(root, ec) || ec) {
        posix::fprintf(stderr, "创建超长目录失败 '%s': %s\n", root.string().c_str(), ec.message().c_str());
        return 1;
    }

    fs::path dataDir = root / "data";
    fs::path testDataDir = root / "test-data";
    if (!aLinkDir(srcData, dataDir) || !aLinkDir(srcTestData, testDataDir))
        return 1;

    posix::printf("AST_DATA_DIR=%s\n", dataDir.string().c_str());
    posix::printf("AST_TEST_DATA_DIR=%s\n", testDataDir.string().c_str());

    // 先在本进程里确认链接后的数据可读，避免带着坏链接去跑整套测试
    if (!aCanReadFile(testDataDir.string() + kTestDataFile))
        return 1;
    if (!aCanReadFile(dataDir.string() + kDataFile))
        return 1;

    // GITHUB_ENV 对后续 step 生效
    const char* envFile = posix::getenv("GITHUB_ENV");
    if (envFile && envFile[0]) {
        cxx::ofstream out(envFile, std::ios::app);
        if (!out.is_open()) {
            posix::fprintf(stderr, "无法写入 GITHUB_ENV '%s'\n", envFile);
            return 1;
        }
        out << "AST_DATA_DIR=" << dataDir.string() << "\n";
        out << "AST_TEST_DATA_DIR=" << testDataDir.string() << "\n";
        out.flush();
        if (!out.good()) {
            posix::fprintf(stderr, "写入 GITHUB_ENV '%s' 失败\n", envFile);
            return 1;
        }
    }
    return 0;
}


int aCheck()
{
    std::string dataDir = aEnvValue("AST_DATA_DIR");
    std::string testDataDir = aEnvValue("AST_TEST_DATA_DIR");
    if (dataDir.empty() || testDataDir.empty()) {
        posix::fprintf(stderr, "AST_DATA_DIR / AST_TEST_DATA_DIR 未设置，无法执行超长目录测试\n");
        return 1;
    }

    // 目录本身超过 MAX_PATH，才能保证其中的数据文件路径也超过 CSPICE 的 255
    if (dataDir.size() < kMinRootLength || testDataDir.size() < kMinRootLength) {
        posix::fprintf(stderr, "数据目录不够长（%u / %u 字符），没有覆盖超长路径\n",
                static_cast<unsigned>(dataDir.size()), static_cast<unsigned>(testDataDir.size()));
        return 1;
    }

    if (!fs::is_directory(dataDir) || !fs::is_directory(testDataDir)) {
        posix::fprintf(stderr, "数据目录不可见：'%s' / '%s'\n", dataDir.c_str(), testDataDir.c_str());
        return 1;
    }

    // aDataDir() 在路径不可见时会静默回退到默认目录，必须显式比较
    if (aDataDir() != dataDir) {
        posix::fprintf(stderr, "aDataDir() 未采用 AST_DATA_DIR：'%s'\n", aDataDir().c_str());
        return 1;
    }
    if (aTestDataDirGet() != testDataDir) {
        posix::fprintf(stderr, "aTestDataDirGet() 未采用 AST_TEST_DATA_DIR：'%s'\n",
                aTestDataDirGet().c_str());
        return 1;
    }

    size_t count = 0;
    fs::path testDataPath(testDataDir);
    for (fs::directory_iterator it(testDataPath); it != fs::directory_iterator(); ++it)
        ++count;
    if (count == 0) {
        posix::fprintf(stderr, "test-data 目录为空：'%s'\n", testDataDir.c_str());
        return 1;
    }

    if (!aCanReadFile(testDataDir + kTestDataFile))
        return 1;
    if (!aCanReadFile(dataDir + kDataFile))
        return 1;

    posix::printf("超长数据目录校验通过\n");
    return 0;
}

} // namespace


int main(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--check") == 0)
            return aCheck();
    }
    return aPrepare();
}
