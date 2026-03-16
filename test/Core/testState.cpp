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

TEST_F(StateTest, ChangeFrame)
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


GTEST_MAIN();


