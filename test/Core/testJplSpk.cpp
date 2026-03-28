///
/// @file      testJplSpk.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-26
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

#include "AstCore/JplSpk.hpp"
#include "AstCore/SpiceApi.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstMath/Vector.hpp"
#include "AstTest/Test.h"


AST_USING_NAMESPACE

TEST(JplSpkTest, open_twice)
{
    Vector3d pos;
    JplSpk spk1;
    JplSpk spk2;
    const std::string spkfile = aTestGetConfigValue("SPK_FILE").toString();

    spk1.open(spkfile);
    spk2.open(spkfile);
    err_t rc1 = spk1.getPosICRF(TimePoint{}, ESpiceId::eJupiter, ESpiceId::eSolarSystemBarycenter, pos);
    spk2.close();
    err_t rc2 = spk1.getPosICRF(TimePoint{}, ESpiceId::eJupiter, ESpiceId::eSolarSystemBarycenter, pos);
    spk1.close();
    err_t rc0 = spk1.getPosICRF(TimePoint{}, ESpiceId::eJupiter, ESpiceId::eSolarSystemBarycenter, pos);

    printf("rc0: %d, rc1: %d, rc2: %d\n", rc0, rc1, rc2);
}


TEST(JplSpkTest, open)
{
    TimeInterval interval;
    std::string spkfile = aTestGetConfigValue("SPK_FILE").toString();

    int count = SpiceApi::Instance()->ktotal("ALL");
    EXPECT_EQ(count, 0);
    JplSpk spk(spkfile);
    err_t rc = spk.getInterval(ESpiceId::eJupiter, interval);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NE(spk.handle(), 0);
    printf("interval: %s\n", interval.toString().c_str());

    std::vector<std::string> names;
    spk.getBodyNames(names);
    EXPECT_GT(names.size(), 0);
    for(const std::string& name : names)
    {
        printf("%s\n", name.c_str());
    }
}


GTEST_MAIN()
