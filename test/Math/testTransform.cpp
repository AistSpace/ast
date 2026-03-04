///
/// @file      testTransform.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-04
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

#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/EarthFrame.hpp"
#include "AstMath/Vector.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

TEST(TransformTest, Rotation)
{
    Rotation rotation;
    TimePoint tp = TimePoint::FromUTC(2000, 1, 2, 3, 4, 5.0);
    aJ2000ToECFTransform(tp, rotation);
    Vector3d vec1{1, 2, 3};
    Vector3d vecRot = rotation.transformVector(vec1);
    Vector3d vec2 = rotation.inverse().transformVector(vecRot);
    printf("vecRot: %f, %f, %f\n", vecRot.x(), vecRot.y(), vecRot.z());
    printf("vec1: %f, %f, %f\n", vec1.x(), vec1.y(), vec1.z());
    printf("vec2: %f, %f, %f\n", vec2.x(), vec2.y(), vec2.z());
    for(int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(vec2[i], vec1[i], 1e-12);
    }
}

TEST(TransformTest, KinematicRotation_inverse)
{
    KinematicRotation kinematicRotation;
    TimePoint tp = TimePoint::FromUTC(2000, 1, 2, 3, 4, 5.0);
    aJ2000ToECFTransform(tp, kinematicRotation);
    Vector3d vec1{1, 2, 3}, vecvel1{4, 5, 6};
    Vector3d vecRot, vecvelRot;
    Vector3d vec2, vecvel2;
    kinematicRotation.transformVectorVelocity(vec1, vecvel1, vecRot, vecvelRot);
    kinematicRotation.inverse().transformVectorVelocity(vecRot, vecvelRot, vec2, vecvel2);
    printf("vecvel1: %f, %f, %f\n", vecvel1.x(), vecvel1.y(), vecvel1.z());
    printf("vecvel2: %f, %f, %f\n", vecvel2.x(), vecvel2.y(), vecvel2.z());
    printf("vec1: %f, %f, %f\n", vec1.x(), vec1.y(), vec1.z());
    printf("vec2: %f, %f, %f\n", vec2.x(), vec2.y(), vec2.z());
    printf("vecRot: %f, %f, %f\n", vecRot.x(), vecRot.y(), vecRot.z()); 
    printf("vecvelRot: %f, %f, %f\n", vecvelRot.x(), vecvelRot.y(), vecvelRot.z()); 
    for(int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(vecvel2[i], vecvel1[i], 1e-12);
        EXPECT_NEAR(vec2[i], vec1[i], 1e-12);
    }
}

TEST(TransformTest, KinematicRotation_compose)
{
    KinematicRotation kinematicRotation1;
    KinematicRotation kinematicRotation2;
    TimePoint tp1 = TimePoint::FromUTC(2010, 1, 2, 3, 4, 5.0);
    TimePoint tp2 = TimePoint::FromUTC(2020, 1, 2, 3, 4, 5.0);
    aJ2000ToECFTransform(tp1, kinematicRotation1);
    aJ2000ToECFTransform(tp2, kinematicRotation2);

    KinematicRotation kinematicRotationComposed1 = kinematicRotation1.composed(kinematicRotation2);
    KinematicRotation kinematicRotationComposed2 = kinematicRotation1;
    kinematicRotationComposed2.compose(kinematicRotation2);
    Vector3d vec{1, 2, 3}, vel{4, 5, 6};
    Vector3d vec1, vel1;
    Vector3d vec2, vel2;
    Vector3d vec3, vel3;
    Vector3d vecTemp, velTemp;

    kinematicRotationComposed1.transformVectorVelocity(vec, vel, vec1, vel1);
    kinematicRotationComposed2.transformVectorVelocity(vec, vel, vec2, vel2);

    kinematicRotation1.transformVectorVelocity(vec, vel, vecTemp, velTemp);
    kinematicRotation2.transformVectorVelocity(vecTemp, velTemp, vec3, vel3);
    
    printf("vec1: %f, %f, %f\n", vec1.x(), vec1.y(), vec1.z());
    printf("vec2: %f, %f, %f\n", vec2.x(), vec2.y(), vec2.z());
    printf("vec3: %f, %f, %f\n", vec3.x(), vec3.y(), vec3.z());

    printf("vel1: %f, %f, %f\n", vel1.x(), vel1.y(), vel1.z());
    printf("vel2: %f, %f, %f\n", vel2.x(), vel2.y(), vel2.z());
    printf("vel3: %f, %f, %f\n", vel3.x(), vel3.y(), vel3.z());

    EXPECT_NEAR(vec1.x(), vec3.x(), 1e-14);
    EXPECT_NEAR(vec1.y(), vec3.y(), 1e-14);
    EXPECT_NEAR(vec1.z(), vec3.z(), 1e-14);
    EXPECT_NEAR(vel1.x(), vel3.x(), 1e-14);
    EXPECT_NEAR(vel1.y(), vel3.y(), 1e-14);
    EXPECT_NEAR(vel1.z(), vel3.z(), 1e-14);
    EXPECT_NEAR(vec2.x(), vec3.x(), 1e-14);
    EXPECT_NEAR(vec2.y(), vec3.y(), 1e-14);
    EXPECT_NEAR(vec2.z(), vec3.z(), 1e-14);
    EXPECT_NEAR(vel2.x(), vel3.x(), 1e-14);
    EXPECT_NEAR(vel2.y(), vel3.y(), 1e-14);
    EXPECT_NEAR(vel2.z(), vel3.z(), 1e-14);

    EXPECT_EQ(vec1.x(), vec2.x());
    EXPECT_EQ(vec1.y(), vec2.y());
    EXPECT_EQ(vec1.z(), vec2.z());
    EXPECT_EQ(vel1.x(), vel2.x());
    EXPECT_EQ(vel1.y(), vel2.y());
    EXPECT_EQ(vel1.z(), vel2.z());

    {
        auto identity = kinematicRotation1.composed(kinematicRotation1.inverse());
        Vector3d vec1{1, 2, 3}, vel1{4,5,6};
        Vector3d vec2, vel2;
        identity.transformVectorVelocity(vec1, vel1, vec2, vel2);
        
        printf("vec1: %f, %f, %f\n", vec1.x(), vec1.y(), vec1.z());
        printf("vec2: %f, %f, %f\n", vec2.x(), vec2.y(), vec2.z());
        printf("vel1: %f, %f, %f\n", vel1.x(), vel1.y(), vel1.z());
        printf("vel2: %f, %f, %f\n", vel2.x(), vel2.y(), vel2.z());

        for(int i = 0; i < 3; i++)
        {
            EXPECT_NEAR(vec1[i], vec2[i], 1e-14);
            EXPECT_NEAR(vel1[i], vel2[i], 1e-14);
        }
    }
}

GTEST_MAIN()

