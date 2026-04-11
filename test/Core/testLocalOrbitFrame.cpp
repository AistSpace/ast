///
/// @file      testLocalOrbitFrame.cpp
/// @brief     局部轨道坐标系测试
/// @details   测试VVLH、LVLH、VNC等局部轨道坐标系的转换矩阵计算
/// @author    axel
/// @date      2026-04-11
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

#include "AstCore/LocalOrbitFrame.hpp"
#include "AstCore/RunTime.hpp"
#include "AstMath/Matrix.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

// 辅助函数：获取矩阵的某列
static Vector3d getMatrixCol(const Matrix3d& m, int col) {
    return Vector3d{m(0, col), m(1, col), m(2, col)};
}

// 辅助函数：检查是否为单位矩阵
static bool isIdentityMatrix(const Matrix3d& m, double tol = 1e-10) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            double expected = (i == j) ? 1.0 : 0.0;
            if (std::abs(m(i, j) - expected) > tol) return false;
        }
    }
    return true;
}

// 辅助函数：检查是否为正交矩阵（M * M^T = I）
static bool isOrthogonalMatrix(const Matrix3d& m, double tol = 1e-10) {
    Matrix3d product = m * m.transpose();
    return isIdentityMatrix(product, tol);
}

TEST(LocalOrbitFrame, FrameToVVLH)
{
    // 正常情况：位置和速度向量构成有效轨道平面
    {
        Vector3d pos{6778.0, 0.0, 0.0};  // km，近地点在+X方向
        Vector3d vel{0.0, 7.5, 0.0};     // km/s，在轨道面内
        
        Matrix3d matrix;
        errc_t rc = aFrameToVVLHMatrix(pos, vel, matrix);
        
        EXPECT_EQ(rc, eNoError);
        // 验证矩阵是正交的
        EXPECT_TRUE(isOrthogonalMatrix(matrix, 1e-10));
        // 验证Z轴指向位置的反方向
        Vector3d zAxis = getMatrixCol(matrix, 2);
        Vector3d expectedZ = -pos.normalized();
        EXPECT_NEAR(zAxis.dot(expectedZ), 1.0, 1e-10);
    }
    
    // 测试VVLH到Frame的逆变换
    {
        Vector3d pos{6778.0, 0.0, 0.0};
        Vector3d vel{0.0, 7.5, 0.0};
        
        Matrix3d toVVLH, fromVVLH;
        aFrameToVVLHMatrix(pos, vel, toVVLH);
        aVVLHToFrameMatrix(pos, vel, fromVVLH);
        
        // 验证互逆性
        Matrix3d product = toVVLH * fromVVLH;
        EXPECT_TRUE(isIdentityMatrix(product, 1e-10));
    }
}

TEST(LocalOrbitFrame, FrameToLVLH)
{
    // 正常情况
    {
        Vector3d pos{6778.0, 0.0, 0.0};
        Vector3d vel{0.0, 7.5, 0.0};
        
        Matrix3d matrix;
        errc_t rc = aFrameToLVLHMatrix(pos, vel, matrix);
        
        EXPECT_EQ(rc, eNoError);
        // 验证矩阵是正交的
        EXPECT_TRUE(isOrthogonalMatrix(matrix, 1e-10));
        // 验证X轴指向位置方向
        Vector3d xAxis = getMatrixCol(matrix, 0);
        Vector3d expectedX = pos.normalized();
        EXPECT_NEAR(xAxis.dot(expectedX), 1.0, 1e-10);
    }
    
    // 测试LVLH到Frame的逆变换
    {
        Vector3d pos{6778.0, 0.0, 0.0};
        Vector3d vel{0.0, 7.5, 0.0};
        
        Matrix3d toLVLH, fromLVLH;
        aFrameToLVLHMatrix(pos, vel, toLVLH);
        aLVLHToFrameMatrix(pos, vel, fromLVLH);
        
        // 验证互逆性
        Matrix3d product = toLVLH * fromLVLH;
        EXPECT_TRUE(isIdentityMatrix(product, 1e-10));
    }
}

TEST(LocalOrbitFrame, FrameToVNC)
{
    // 正常情况
    {
        Vector3d pos{6778.0, 0.0, 0.0};
        Vector3d vel{0.0, 7.5, 0.0};
        
        Matrix3d matrix;
        errc_t rc = aFrameToVNCMatrix(pos, vel, matrix);
        
        EXPECT_EQ(rc, eNoError);
        // 验证矩阵是正交的
        EXPECT_TRUE(isOrthogonalMatrix(matrix, 1e-10));
        // 验证X轴指向速度方向
        Vector3d xAxis = getMatrixCol(matrix, 0);
        Vector3d expectedX = vel.normalized();
        EXPECT_NEAR(xAxis.dot(expectedX), 1.0, 1e-10);
    }
    
    // 测试VNC到Frame的逆变换
    {
        Vector3d pos{6778.0, 0.0, 0.0};
        Vector3d vel{0.0, 7.5, 0.0};
        
        Matrix3d toVNC, fromVNC;
        aFrameToVNCMatrix(pos, vel, toVNC);
        aVNCToFrameMatrix(pos, vel, fromVNC);
        
        // 验证互逆性
        Matrix3d product = toVNC * fromVNC;
        EXPECT_TRUE(isIdentityMatrix(product, 1e-10));
    }
}

TEST(LocalOrbitFrame, ErrorCases)
{
    // 测试pos和vel方向相同的情况（叉积为零）
    {
        Vector3d pos{1.0, 0.0, 0.0};
        Vector3d vel{1.0, 0.0, 0.0};  // 同方向
        
        Matrix3d matrix;
        matrix.setIdentity();
        errc_t rc = aFrameToVVLHMatrix(pos, vel, matrix);
        
        EXPECT_EQ(rc, eErrorInvalidParam);
    }
    
    // 测试pos为零向量
    {
        Vector3d pos{0.0, 0.0, 0.0};
        Vector3d vel{1.0, 0.0, 0.0};
        
        Matrix3d matrix;
        matrix.setIdentity();
        errc_t rc = aFrameToLVLHMatrix(pos, vel, matrix);
        
        EXPECT_EQ(rc, eErrorInvalidParam);
    }
    
    // 测试vel为零向量
    {
        Vector3d pos{1.0, 0.0, 0.0};
        Vector3d vel{0.0, 0.0, 0.0};
        
        Matrix3d matrix;
        matrix.setIdentity();
        errc_t rc = aFrameToVNCMatrix(pos, vel, matrix);
        
        EXPECT_EQ(rc, eErrorInvalidParam);
    }
}

// ENU测试已移除：aFrameToENUMatrix 和 aENUToFrameMatrix 函数未实现
