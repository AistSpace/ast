///
/// @file      testPlanetPoint.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-12
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
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstCore/PlanetPoint.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/JplDe.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstMath/Vector.hpp"
#include "AstTest/Test.hpp"

AST_USING_NAMESPACE

TEST(PlanetPointTest, getPosICRF)
{
    aInitialize();
    TimePoint tp = TimePoint::FromUTC(2026, 3, 12, 0, 0, 0);
    #define _AST_TEST_PLANET_POS(NAME) \
    { \
        Vector3d pos1, pos2; \
        err_t rc = aJplDeGetPosICRF(tp, JplDe::e##NAME, JplDe::eSSBarycenter, pos1); \
        EXPECT_EQ(rc, eNoError); \
        rc = a##NAME##PosInICRF(tp, pos2); \
        EXPECT_EQ(rc, eNoError); \
        for(int i = 0; i < 3; i++) \
        { \
            EXPECT_EQ(pos1[i], pos2[i]); \
        }\
    }\

    _AST_TEST_PLANET_POS(Mercury);
    _AST_TEST_PLANET_POS(Venus);
    _AST_TEST_PLANET_POS(Earth);
    _AST_TEST_PLANET_POS(Mars);
    _AST_TEST_PLANET_POS(Jupiter);
    _AST_TEST_PLANET_POS(Saturn);
    _AST_TEST_PLANET_POS(Uranus);
    _AST_TEST_PLANET_POS(Neptune);
    _AST_TEST_PLANET_POS(Moon);
    _AST_TEST_PLANET_POS(Sun);
    _AST_TEST_PLANET_POS(EMBarycenter);
}


TEST(PlanetPointTest, getPosVelICRF)
{
    aInitialize();
    TimePoint tp = TimePoint::FromUTC(2026, 3, 12, 0, 0, 0);
    #define _AST_TEST_PLANET_POSVEL(NAME) \
    { \
        Vector3d pos1, pos2; \
        Vector3d vel1, vel2; \
        err_t rc = aJplDeGetPosVelICRF(tp, JplDe::e##NAME, JplDe::eSSBarycenter, pos1, vel1); \
        EXPECT_EQ(rc, eNoError); \
        rc = a##NAME##PosVelInICRF(tp, pos2, vel2); \
        EXPECT_EQ(rc, eNoError); \
        for(int i = 0; i < 3; i++) \
        { \
            EXPECT_EQ(pos1[i], pos2[i]); \
            EXPECT_EQ(vel1[i], vel2[i]); \
        }\
    }\

    _AST_TEST_PLANET_POSVEL(Mercury);
    _AST_TEST_PLANET_POSVEL(Venus);
    _AST_TEST_PLANET_POSVEL(Earth);
    _AST_TEST_PLANET_POSVEL(Mars);
    _AST_TEST_PLANET_POSVEL(Jupiter);
    _AST_TEST_PLANET_POSVEL(Saturn);
    _AST_TEST_PLANET_POSVEL(Uranus);
    _AST_TEST_PLANET_POSVEL(Neptune);
    _AST_TEST_PLANET_POSVEL(Moon);
    _AST_TEST_PLANET_POSVEL(Sun);
    _AST_TEST_PLANET_POSVEL(EMBarycenter);
}

GTEST_MAIN()
