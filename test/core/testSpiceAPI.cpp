///
/// @file      testSpiceApi.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-22
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

#include "ast/SpiceAPI.hpp"
#include "ast/Test.h"
#include "ast/RunTime.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/FileSystem.hpp"
#include <fstream>

AST_USING_NAMESPACE

namespace {

// 暴露受保护的 makeSpicePath 以便直接做单元测试
class TestableSpiceAPI : public SpiceAPI
{
public:
    using SpiceAPI::makeSpicePath;
};

} // namespace

TEST(SpiceAPI, LoadUnload)
{
    SpiceAPI api;
    const auto libpaths = aTestGetConfigStringVector("CSPICE_LIBS");
    errc_t rc = api.tryload(libpaths);
    EXPECT_FALSE(api.failed());
    EXPECT_EQ(rc, eNoError);
    {
        SpiceAPI api2;
        rc = api2.tryload(libpaths);
        EXPECT_EQ(rc, eNoError);
        EXPECT_FALSE(api2.failed());
    }
    EXPECT_FALSE(api.failed());
}


TEST(SpiceAPI, Instance)
{
    EXPECT_TRUE(SpiceAPI::Instance()->isLoaded());
    bool failed = SpiceAPI::Instance()->failed();
    EXPECT_FALSE(failed);
}


TEST(SpiceAPI, HashBodyEphemeris)
{
    if(aIsCI())
        return;
    aInitialize();
    EXPECT_TRUE(aSpiceHasBodyEphemeris(ESpiceId::eEarth));
    EXPECT_TRUE(aSpiceHasBodyEphemeris(ESpiceId::eMercury));
    EXPECT_TRUE(aSpiceHasBodyEphemeris(ESpiceId::eGanymede));
    EXPECT_FALSE(aSpiceHasBodyEphemeris(12345678910));
}


// 测试 CSPICE 路径预处理：普通路径原样使用，超长路径映射到临时目录下的短路径别名
TEST(SpiceAPI, MakeSpicePath)
{
    namespace fs = _AST fs_simple;

    TestableSpiceAPI api;

    // 空路径不产生输出
    std::string spicePath = "not empty";
    api.makeSpicePath("", spicePath);
    EXPECT_TRUE(spicePath.empty());

    // 普通短路径直接交给 CSPICE
    api.makeSpicePath("kernel.tls", spicePath);
    EXPECT_EQ(spicePath, "kernel.tls");

    // 构造超过 CSPICE FILSIZ(255) 的目录
    fs::path base = fs::temp_directory_path() / "ast_test_spice_path";
    fs::remove_all(base);
    fs::path longDir = base;
    while (longDir.string().size() < 300)
    {
        longDir = longDir / std::string(60, 'd');
    }
    ASSERT_TRUE(fs::create_directories(longDir));
    ASSERT_GT(longDir.string().size(), 255u);

    fs::path longFile = longDir / "kernel.tls";
    std::string mapped;
    api.makeSpicePath(longFile.string(), mapped);
    ASSERT_LE(mapped.size(), 255u);
    EXPECT_NE(mapped, longFile.string());
    // 别名目录应指向原目录：往短路径里写文件，能在原长路径下看到
    EXPECT_TRUE(fs::is_directory(fs::path(mapped).parent_path()));
    {
        std::ofstream file(mapped);
        ASSERT_TRUE(file.good());
        file << "dummy kernel";
    }
    EXPECT_TRUE(fs::exists(longFile));

    // 同一目录下的文件复用同一个别名目录
    std::string mapped2;
    api.makeSpicePath((longDir / "kernel2.tls").string(), mapped2);
    EXPECT_EQ(fs::path(mapped2).parent_path(), fs::path(mapped).parent_path());

    // 析构时清理别名根目录
    fs::remove_all(base);
}


GTEST_MAIN();



