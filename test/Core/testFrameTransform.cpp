///
/// @file      testFrameTransform.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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

#include "AstCore/FrameTransform.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/MoonFrame.hpp"
#include "AstCore/JplDe.hpp"
#include "AstMath/Transform.hpp"
#include "AstMath/KinematicTransform.hpp"
#include "AstMath/Matrix.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE


TEST(FrameTransformTest, Transform_Case1)
{
    aInitialize();
    KinematicTransform transform;
    auto earth = aGetEarth();
    auto moon = aGetMoon();
    auto earthICRF = earth->makeFrameICRF();
    auto moonICRF = moon->makeFrameICRF();
    TimePoint tp = TimePoint::FromUTC(2026, 3, 5, 0, 0, 0);
    err_t rc = aFrameTransform(earthICRF, moonICRF, tp, transform);
    EXPECT_FALSE(rc);
    Vector3d pos{1000.0, 2000.0, 3000.0}, vel{1000, 2000, 3000};
    Vector3d posMoon1, velMoon1, posMoon2, velMoon2;
    /*!
    @bug 
    通过 aFrameTransform 获取的月球位置数值精度降低了
    因为他将月球星历转换到了ICRF，而其原始星历是相对于地球的
    */
    transform.transformPositionVelocity(pos, vel, posMoon1, velMoon1);
    rc = aEarthICRFToMoonICRF(tp, pos, vel, posMoon2, velMoon2);
    EXPECT_FALSE(rc);
    printf("posMoon1 = %s\nposMoon2 = %s\n", posMoon1.toString().c_str(), posMoon2.toString().c_str());
    printf("velMoon1 = %s\nvelMoon2 = %s\n", velMoon1.toString().c_str(), velMoon2.toString().c_str());
    for(int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(posMoon1[i], posMoon2[i], posMoon2[i] * 1e-12);
        EXPECT_NEAR(velMoon1[i], velMoon2[i], velMoon2[i] * 1e-12);
    }
    
    {
        Vector3d posMoon, velMoon;
        Vector3d posEarth, velEarth;
        Vector3d posMoonInEarth1, velMoonInEarth1;
        Vector3d posMoonInEarth2, velMoonInEarth2;
        aJplDeGetPosVelICRF(tp, JplDe::eMoon, JplDe::eEarth, posMoonInEarth1, velMoonInEarth1);
        aJplDeGetPosVelICRF(tp, JplDe::eEarth, JplDe::eSSBarycenter, posEarth, velEarth);
        aJplDeGetPosVelICRF(tp, JplDe::eMoon, JplDe::eSSBarycenter, posMoon, velMoon);
        posMoonInEarth2 = posMoon - posEarth;
        velMoonInEarth2 = velMoon - velEarth;
        printf("posMoonInEarth1 = %s\nposMoonInEarth2 = %s\n", posMoonInEarth1.toString().c_str(), posMoonInEarth2.toString().c_str());
        printf("velMoonInEarth1 = %s\nvelMoonInEarth2 = %s\n", velMoonInEarth1.toString().c_str(), velMoonInEarth2.toString().c_str());
    }
}


TEST(FrameTransformTest, Transform_Case2)
{
    aInitialize();
    auto mars = aGetMars();
    auto jupiter = aGetJupiter();
    {
        auto marsInertial = mars->makeFrameInertial();
        auto jupiterInertial = jupiter->makeFrameInertial();
        TimePoint tp = TimePoint::FromUTC(2026, 3, 5, 0, 0, 0);
        Transform transform;
        err_t rc = aFrameTransform(marsInertial, jupiterInertial, tp, transform);
        EXPECT_FALSE(rc);
        Vector3d posMars{1000.0, 2000.0, 3000.0};
        Vector3d posJupiter{};
        transform.transformPosition(posMars, posJupiter);
        printf("posJupiter = %s\n", posJupiter.toString().c_str());
    }
    {
        auto marsICRF = mars->makeFrameICRF();
        auto jupiterICRF = jupiter->makeFrameICRF();
        TimePoint tp = TimePoint::FromUTC(2026, 3, 5, 0, 0, 0);
        Transform transform;
        err_t rc = aFrameTransform(marsICRF, jupiterICRF, tp, transform);
        EXPECT_FALSE(rc);
        Vector3d posMars{1000.0, 2000.0, 3000.0};
        Vector3d posJupiter{};
        transform.transformPosition(posMars, posJupiter);
        printf("posJupiter = %s\n", posJupiter.toString().c_str());
    }
}


GTEST_MAIN()