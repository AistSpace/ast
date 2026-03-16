///
/// @file      testState.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-15
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

#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/SOFA.hpp"
#include "AstCore/EOP.hpp"
#include "AstCore/EarthFrame.hpp"
#include "AstUtil/Literals.hpp"
#include "AstTest/Test.hpp"

AST_USING_NAMESPACE


class StateTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        aInitialize();
        aDataContext_GetEOP()->unload();    
    }
    void TearDown() override
    {
        aUninitialize();
    }
};

TEST_F(StateTest, ChangeFrameBasic)
{
    // 测试从ICRF到TOD的转换
    {
        err_t rc;
        auto cartState = StateCartesian::MakeShared();
        SharedPtr<CelestialBody> earth = aGetEarth();
        
        auto icrfFrame = earth->makeFrame(aAxesICRF());
        auto todFrame = earth->makeFrame(aAxesTOD());
        rc = cartState->setState({6678137_m, 0.0, 0.0, 0.0, 6789.5_m/s, 3686.4_m/s});
        EXPECT_EQ(rc, 0);
        cartState->setStateEpoch(TimePoint::FromUTC(2026, 3, 15, 0, 0, 0));
        cartState->setFrame(icrfFrame);
        rc = cartState->changeFrame(todFrame);
        EXPECT_EQ(rc, 0);
        CartState state;
        rc = cartState->getState(state);
        EXPECT_EQ(rc, 0);
        CartState stateExpect {
            6677.9993219814050462_km, 39.3297108095184242_km, 17.0885994905810570_km,
            -0.0494184559469364_km/sec, 6.7891934257351290_km/sec, 3.6866333766957053_km/sec
        };
        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(state[i], stateExpect[i], 1e-3);
            EXPECT_NEAR(state[3+i], stateExpect[3+i], 1e-6);
        }
    }
}

TEST_F(StateTest, ChangeFrameWithEpoch)
{
    // 测试从TOE到MOE的转换
    {
        err_t rc;
        auto cartState = StateCartesian::MakeShared();
        SharedPtr<CelestialBody> earth = aGetEarth();
        auto coordEpoch = TimePoint::FromUTC(1980, 12, 27, 0, 0, 0);
        auto stateEpoch = TimePoint::FromUTC(2026, 3, 15, 0, 0, 0);
        auto toe = earth->makeFrameTOE(coordEpoch);
        auto moe = earth->makeFrameMOE(coordEpoch);
        CartState state0{6678137_m, 0.0, 0.0, 0.0, 6789.5_m/s, 3686.4_m/s};
        rc = cartState->setState(state0);
        EXPECT_EQ(rc, 0);
        cartState->setStateEpoch(stateEpoch);
        cartState->setFrame(toe);
        rc = cartState->changeFrame(moe);
        EXPECT_EQ(rc, 0);
        CartState state1;
        rc = cartState->getState(state1);
        EXPECT_EQ(rc, 0);
        printf("state1: %f %f %f %f %f %f\n", state1[0], state1[1], state1[2], state1[3], state1[4], state1[5]);
        CartState stateExpect {
            6678.1369879813173611_km, 0.3675873390982545_km, 0.1593874602611343_km,
            -0.0004616993065655_km/sec, 6.7893791093392464_km/sec, 3.6866226151454065_km/sec
        };
        Rotation rotation;
        aMODToTODTransform(coordEpoch, rotation);
        rotation = rotation.inverse();
        CartState state2;
        state2.pos() = rotation.transformVector(state0.pos());
        state2.vel() = rotation.transformVector(state0.vel());
        printf("state2: %f %f %f %f %f %f\n", state2[0], state2[1], state2[2], state2[3], state2[4], state2[5]);

        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(state1[i], stateExpect[i], 1e-5);
            EXPECT_NEAR(state2[i], stateExpect[i], 1e-5);
            EXPECT_NEAR(state1[3+i], stateExpect[3+i], 1e-8);
            EXPECT_NEAR(state2[3+i], stateExpect[3+i], 1e-8);
        }
    }
    // 测试从TOE到MOD的转换
    {
        err_t rc;
        auto cartState = StateCartesian::MakeShared();
        SharedPtr<CelestialBody> earth = aGetEarth();
        auto coordEpoch = TimePoint::FromUTC(1980, 12, 27, 0, 0, 0);
        auto stateEpoch = TimePoint::FromUTC(2026, 3, 15, 0, 0, 0);
        auto toe = earth->makeFrameTOE(coordEpoch);
        auto mod = earth->makeFrameMOD();
        CartState state0{6678137_m, 0.0, 0.0, 0.0, 6789.5_m/s, 3686.4_m/s};
        rc = cartState->setState(state0);
        EXPECT_EQ(rc, 0);
        cartState->setStateEpoch(stateEpoch);
        cartState->setFrame(toe);
        rc = cartState->changeFrame(mod);
        EXPECT_EQ(rc, 0);
        CartState state1;
        rc = cartState->getState(state1);
        EXPECT_EQ(rc, 0);
        printf("state1: %f %f %f %f %f %f\n", state1[0], state1[1], state1[2], state1[3], state1[4], state1[5]);
        CartState stateExpect {
            6677.7267929937161171_km, 67.8861414016167259_km, 29.4981475428268247_km,
            -0.0853012283266688_km/sec, 6.7889455414363677_km/sec, 3.6864342318637973_km/sec
        };

        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(state1[i], stateExpect[i], 1e-5);
            EXPECT_NEAR(state1[3+i], stateExpect[3+i], 1e-8);
        }
    }
}



GTEST_MAIN();


