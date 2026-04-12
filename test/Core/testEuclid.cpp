///
/// @file      testEuclid.cpp
/// @brief     Euclid模块单元测试
/// @details   测试Axes、Frame、Point等几何基类的功能
/// @author    coze
/// @date      2026-04-12
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstCore/Axes.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/AxesRoot.hpp"
#include "AstCore/AxesICRF.hpp"
#include "AstCore/AxesTransform.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/EarthFrame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Point.hpp"
#include "AstMath/Transform.hpp"
#include "AstMath/KinematicTransform.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/Vector.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE

// ============================================
// Axes类测试
// ============================================

TEST(AxesTest, GetDepth)
{
    // 测试根轴系深度为1
    HAxes root = aAxesRoot();
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getDepth(), 1);
    
    aInitialize();
    
    // ICRF轴系深度应该大于1（有父轴系）
    HAxes icrf = aAxesICRF();
    ASSERT_NE(icrf, nullptr);
    EXPECT_GT(icrf->getDepth(), 1);
}

TEST(AxesTest, GetAncestor)
{
    aInitialize();
    
    HAxes icrf = aAxesICRF();
    HAxes ecf = aAxesECF();
    
    ASSERT_NE(icrf, nullptr);
    ASSERT_NE(ecf, nullptr);
    
    // 获取根轴系
    HAxes root = aAxesRoot();
    ASSERT_NE(root, nullptr);
    
    // ICRF的祖先应该包含根轴系
    Axes* ancestor = icrf->getAncestor(0);
    EXPECT_EQ(ancestor, icrf.get());
    
    // 深度足够时应该能找到根
    Axes* foundRoot = icrf->getAncestor(icrf->getDepth() - 1);
    EXPECT_EQ(foundRoot, root.get());
}

TEST(AxesTest, AxesTransformSelfToSelf)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    HAxes icrf = aAxesICRF();
    
    // 测试自变换（应该得到单位旋转）
    Rotation rotation;
    errc_t rc = icrf->getTransformTo(icrf.get(), tp, rotation);
    EXPECT_EQ(rc, eNoError);
    
    Matrix3d mat = rotation.getMatrix();
    // 单位矩阵验证
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            EXPECT_NEAR(mat(i, j), expected, 1e-14);
        }
    }
}

TEST(AxesTest, AxesTransformToAncestor)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    HAxes icrf = aAxesICRF();
    HAxes ecf = aAxesECF();
    
    // 测试从子轴系到父轴系的变换
    Rotation rotation1, rotation2;
    
    // 方法1：使用 getTransformTo
    errc_t rc1 = icrf->getTransformTo(ecf.get(), tp, rotation1);
    EXPECT_EQ(rc1, eNoError);
    
    // 方法2：使用 aAxesTransform
    errc_t rc2 = aAxesTransform(icrf.get(), ecf.get(), tp, rotation2);
    EXPECT_EQ(rc2, eNoError);
    
    // 两种方法结果应该一致
    Matrix3d m1 = rotation1.getMatrix();
    Matrix3d m2 = rotation2.getMatrix();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_NEAR(m1(i, j), m2(i, j), 1e-14);
        }
    }
}

TEST(AxesTest, AxesTransformFromParent)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    HAxes icrf = aAxesICRF();
    HAxes ecf = aAxesECF();
    
    // 测试 getTransformFrom（从父轴系到当前轴系）
    Rotation rotationTo, rotationFrom;
    
    errc_t rc1 = ecf->getTransformTo(icrf.get(), tp, rotationTo);
    EXPECT_EQ(rc1, eNoError);
    
    errc_t rc2 = ecf->getTransformFrom(icrf.get(), tp, rotationFrom);
    EXPECT_EQ(rc2, eNoError);
    
    // 两个旋转变换应该是互逆的
    Matrix3d mTo = rotationTo.getMatrix();
    Matrix3d mFrom = rotationFrom.getMatrix();
    Matrix3d product = mTo * mFrom;
    
    // product 应该接近单位矩阵
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            EXPECT_NEAR(product(i, j), expected, 1e-10);
        }
    }
}

TEST(AxesTest, KinematicRotation)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    HAxes icrf = aAxesICRF();
    HAxes ecf = aAxesECF();
    
    // 测试运动学旋转变换
    KinematicRotation krot1, krot2;
    
    errc_t rc1 = ecf->getTransformTo(icrf.get(), tp, krot1);
    EXPECT_EQ(rc1, eNoError);
    
    errc_t rc2 = aAxesTransform(ecf.get(), icrf.get(), tp, krot2);
    EXPECT_EQ(rc2, eNoError);
    
    // 验证旋转矩阵的正交性
    Matrix3d m1 = krot1.getMatrix();
    Matrix3d m1_t = m1.transpose();
    Matrix3d identity = m1 * m1_t;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            EXPECT_NEAR(identity(i, j), expected, 1e-14);
        }
    }
}

TEST(AxesTest, AxesTransformToMatrix)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    HAxes icrf = aAxesICRF();
    HAxes ecf = aAxesECF();
    
    // 测试直接获取变换矩阵
    Matrix3d matrix;
    errc_t rc = aAxesTransform(ecf.get(), icrf.get(), tp, matrix);
    EXPECT_EQ(rc, eNoError);
    
    // 验证正交矩阵性质
    Matrix3d product = matrix * matrix.transpose();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            EXPECT_NEAR(product(i, j), expected, 1e-14);
        }
    }
}

// ============================================
// Frame类测试
// ============================================

TEST(FrameTest, GetParent)
{
    aInitialize();
    
    auto earth = aGetEarth();
    auto eci = earth->makeFrameICRF();
    
    ASSERT_NE(eci, nullptr);
    
    // ICRF的父坐标系应该不为空
    Frame* parent = eci->getParent();
    EXPECT_NE(parent, nullptr);
}

TEST(FrameTest, GetBody)
{
    aInitialize();
    
    auto earth = aGetEarth();
    auto eci = earth->makeFrameICRF();
    
    ASSERT_NE(eci, nullptr);
    
    // 获取Frame关联的天体
    CelestialBody* body = eci->getBody();
    EXPECT_NE(body, nullptr);
    EXPECT_EQ(body, earth);
}

TEST(FrameTest, GetGM)
{
    aInitialize();
    
    auto earth = aGetEarth();
    auto eci = earth->makeFrameICRF();
    
    ASSERT_NE(eci, nullptr);
    
    // 获取引力参数
    double gm = eci->getGM();
    double earthGM = earth->getGM();
    EXPECT_DOUBLE_EQ(gm, earthGM);
    EXPECT_GT(gm, 0.0);
}

TEST(FrameTest, FrameTransformSelfToSelf)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    auto earth = aGetEarth();
    auto eci = earth->makeFrameICRF();
    
    // 自变换应该得到零位移和单位旋转
    Transform transform;
    errc_t rc = eci->getTransformTo(eci.get(), tp, transform);
    EXPECT_EQ(rc, eNoError);
    
    // 验证平移分量为零
    Vector3d trans = transform.getTranslation();
    EXPECT_NEAR(trans.norm(), 0.0, 1e-14);
    
    // 验证旋转分量为单位矩阵
    Matrix3d rot = transform.getRotation().getMatrix();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            EXPECT_NEAR(rot(i, j), expected, 1e-14);
        }
    }
}

TEST(FrameTest, KinematicTransform)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    auto earth = aGetEarth();
    auto eci = earth->makeFrameICRF();
    auto ecf = earth->makeFrameFixed();
    
    // 测试运动学变换（包含位置和速度）
    KinematicTransform transform;
    errc_t rc = eci->getTransformTo(ecf.get(), tp, transform);
    EXPECT_EQ(rc, eNoError);
    
    // 验证旋转变换的正交性
    const Transform& basicTransform = transform.getTransform();
    Matrix3d rot = basicTransform.getRotation().getMatrix();
    Matrix3d rot_t = rot.transpose();
    Matrix3d identity = rot * rot_t;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            EXPECT_NEAR(identity(i, j), expected, 1e-14);
        }
    }
}

TEST(FrameTest, FrameTransformPosition)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    auto earth = aGetEarth();
    auto eci = earth->makeFrameICRF();
    auto ecf = earth->makeFrameFixed();
    
    // 测试坐标系间的位置变换
    Transform transform;
    errc_t rc = aFrameTransform(eci.get(), ecf.get(), tp, transform);
    EXPECT_EQ(rc, eNoError);
    
    // 测试位置向量变换
    Vector3d pos1{1000.0, 2000.0, 3000.0};
    Vector3d pos2;
    transform.transformPosition(pos1, pos2);
    
    // 逆变换验证
    Vector3d pos1Back;
    transform.inverse().transformPosition(pos2, pos1Back);
    
    EXPECT_NEAR(pos1.x(), pos1Back.x(), 1e-10);
    EXPECT_NEAR(pos1.y(), pos1Back.y(), 1e-10);
    EXPECT_NEAR(pos1.z(), pos1Back.z(), 1e-10);
}

// ============================================
// Point类测试
// ============================================

TEST(PointTest, ToBody)
{
    aInitialize();
    
    auto earth = aGetEarth();
    auto ecf = earth->makeFrameFixed();
    
    // Frame的origin应该能转换为天体
    Point* origin = ecf->getOrigin();
    ASSERT_NE(origin, nullptr);
    
    CelestialBody* body = origin->toBody();
    EXPECT_NE(body, nullptr);
    EXPECT_EQ(body, earth);
}

TEST(PointTest, GetFrame)
{
    aInitialize();
    
    auto earth = aGetEarth();
    auto ecf = earth->makeFrameFixed();
    
    Point* origin = ecf->getOrigin();
    ASSERT_NE(origin, nullptr);
    
    Frame* frame = origin->getFrame();
    EXPECT_NE(frame, nullptr);
}

TEST(PointTest, GetPosVelIn)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    auto earth = aGetEarth();
    auto ecf = earth->makeFrameFixed();
    
    Point* origin = ecf->getOrigin();
    ASSERT_NE(origin, nullptr);
    
    // 测试获取位置和速度
    Vector3d pos, vel;
    errc_t rc = origin->getPosVelIn(ecf.get(), tp, pos, vel);
    EXPECT_EQ(rc, eNoError);
    
    // 原点在ECF坐标系中的位置应为零向量
    EXPECT_NEAR(pos.norm(), 0.0, 1e-14);
}

TEST(PointTest, GetPosInSameFrame)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    auto earth = aGetEarth();
    auto ecf = earth->makeFrameFixed();
    
    Point* origin = ecf->getOrigin();
    ASSERT_NE(origin, nullptr);
    
    // 在ECF中，原点位置应为零
    Vector3d posECF;
    errc_t rc1 = origin->getPosIn(ecf.get(), tp, posECF);
    EXPECT_EQ(rc1, eNoError);
    EXPECT_NEAR(posECF.norm(), 0.0, 1e-14);
}

// ============================================
// 边界情况和错误处理测试
// ============================================

TEST(ErrorHandling, AxesTransformNullInput)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    Rotation rotation;
    
    // 空指针输入应该返回错误码
    errc_t rc1 = aAxesTransform(nullptr, nullptr, tp, rotation);
    EXPECT_NE(rc1, eNoError);
    
    HAxes icrf = aAxesICRF();
    errc_t rc2 = aAxesTransform(nullptr, icrf.get(), tp, rotation);
    EXPECT_NE(rc2, eNoError);
    
    errc_t rc3 = aAxesTransform(icrf.get(), nullptr, tp, rotation);
    EXPECT_NE(rc3, eNoError);
}

TEST(ErrorHandling, FrameTransformNullInput)
{
    aInitialize();
    
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    Transform transform;
    
    // 空指针输入应该返回错误码
    errc_t rc1 = aFrameTransform(nullptr, nullptr, tp, transform);
    EXPECT_NE(rc1, eNoError);
    
    auto earth = aGetEarth();
    auto eci = earth->makeFrameICRF();
    
    errc_t rc2 = aFrameTransform(nullptr, eci.get(), tp, transform);
    EXPECT_NE(rc2, eNoError);
    
    errc_t rc3 = aFrameTransform(eci.get(), nullptr, tp, transform);
    EXPECT_NE(rc3, eNoError);
}

GTEST_MAIN()
