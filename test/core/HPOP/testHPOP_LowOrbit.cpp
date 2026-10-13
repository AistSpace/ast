///
/// @file      testHPOP_LowOrbit.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/HPOP.hpp"
#include "ast/HPOPForceModel.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/Test.hpp"
#include "ast/Literals.hpp"
#include "ast/ODEIntegrator.hpp"


AST_USING_NAMESPACE


TEST(HPOPTest, LowOrbit)
{
    auto earth = aGetBody("Earth");

    SpacecraftParam spacecraftParam;
    HPOPForceModel forceModel;
    forceModel.setCentralBody(earth);
    forceModel.useDrag(true);
    forceModel.gravity().model_ = "EGM2008";

    for(auto densityModel : {
            EAtmDensityModel::e1976Standard, EAtmDensityModel::eHarrisPriester,
            EAtmDensityModel::eJacchia60, EAtmDensityModel::eJacchia70, EAtmDensityModel::eJacchia71,
            EAtmDensityModel::eJacchiaRoberts, 
            EAtmDensityModel::eMSIS1986, EAtmDensityModel::eMSISE1990, EAtmDensityModel::eNRLMSISE2000,
            EAtmDensityModel::eCIRA72, EAtmDensityModel::eDTM2012,
        }
    )
    {
        printf("\n---------------------------------\n");
        printf("Density model: %s\n", toString(densityModel).c_str());

        forceModel.drag().atmDensityModel_ = densityModel;

        OrbElem orbElem{kEarthRadius + 200_km, 0, 28.5_deg, 0_deg, 0_deg, 0_deg};
        CartState initState;
        aOrbElemToCart(orbElem, earth->getGM(), initState.pos(), initState.vel());

        HPOP hpop;
        hpop.setForceModel(forceModel);
        hpop.setSpacecraftParam(spacecraftParam);

        auto start = "12 Oct 2026 04:00:00.000"_utc;
        auto stop = start + 30_day;
        auto state = initState;
        errc_t rc = hpop.propagate(start, stop, state);
        EXPECT_NE(rc, eNoError);
        EXPECT_GT(stop, start);
        EXPECT_TRUE(!std::isnan(state[0]));
        EXPECT_TRUE(!std::isnan(state[1]));
        EXPECT_TRUE(!std::isnan(state[2]));
        EXPECT_TRUE(!std::isnan(state[3]));
        EXPECT_TRUE(!std::isnan(state[4]));
        EXPECT_TRUE(!std::isnan(state[5]));

        printf("init time  : %s\n", start.toString().c_str());
        printf("init  pos  : %s\n", initState.pos().toString().c_str());
        printf("init  vel  : %s\n", initState.vel().toString().c_str());

        printf("final time1: %s\n", stop.toString(16).c_str());
        printf("final pos1 : %s\n", state.pos().toString().c_str());
        printf("final vel1 : %s\n", state.vel().toString().c_str());


        HPOP hpop2;
        hpop2.setForceModel(forceModel);
        hpop2.setSpacecraftParam(spacecraftParam);

        auto state2 = initState;
        auto stop2 = stop;
        rc = hpop2.propagate(start, stop2, state2);
        EXPECT_EQ(rc, eNoError);
        printf("final time2: %s\n", stop2.toString(16).c_str());
        printf("final pos2 : %s\n", state2.pos().toString().c_str());
        printf("final vel2 : %s\n", state2.vel().toString().c_str());
        printf("alt: %.24g km\n", (state2.pos().norm() - kEarthRadius)/1_km);
        printf("\n");

        EXPECT_EQ(stop, stop2);
        EXPECT_NEAR(state[0], state2[0], 1e-6);
        EXPECT_NEAR(state[1], state2[1], 1e-6);
        EXPECT_NEAR(state[2], state2[2], 1e-6);
        EXPECT_NEAR(state[3], state2[3], 1e-9);
        EXPECT_NEAR(state[4], state2[4], 1e-9);
        EXPECT_NEAR(state[5], state2[5], 1e-9);
    }
    
}


GTEST_MAIN()