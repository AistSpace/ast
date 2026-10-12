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


AST_USING_NAMESPACE


TEST(HPOPTest, LowOrbit)
{
    auto earth = aGetBody("Earth");

    SpacecraftParam spacecraftParam;
    HPOPForceModel forceModel;
    forceModel.setCentralBody(earth);
    forceModel.useDrag(true);
    forceModel.gravity().model_ = "EGM2008";

    for(auto densityModel : {EAtmDensityModel::eJacchiaRoberts, EAtmDensityModel::eMSIS1986})
    {
        forceModel.drag().atmDensityModel_ = densityModel;

        OrbElem orbElem{6678137_m, 0, 28.5_deg, 0_deg, 0_deg, 0_deg};
        Vector3d pos, vel;
        aOrbElemToCart(orbElem, earth->getGM(), pos, vel);

        HPOP hpop;
        hpop.setForceModel(forceModel);
        hpop.setSpacecraftParam(spacecraftParam);

        auto start = "12 Oct 2026 04:00:00.000"_utc;
        auto stop = start + 30_day;
        errc_t rc = hpop.propagate(start, stop, pos, vel);
        printf("Density model: %s\n", toString(densityModel).c_str());
        printf("pos: %s\n", pos.toString().c_str());
        printf("vel: %s\n", vel.toString().c_str());
        printf("\n");
    }
    
}


GTEST_MAIN()