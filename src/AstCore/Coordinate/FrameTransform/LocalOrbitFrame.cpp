///
/// @file      LocalOrbitFrame.cpp
/// @brief     局部轨道坐标系
/// @details   局部轨道坐标系是一种基于位置或速度向量的坐标系
/// @author    axel
/// @date      2026-01-13
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

#include "LocalOrbitFrame.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/KinematicTransform.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

namespace {

/// @brief 以三个轴向量为行填充旋转矩阵
A_ALWAYS_INLINE void setMatrixRows(Matrix3d& matrix, const Vector3d& x, const Vector3d& y, const Vector3d& z)
{
    matrix(0, 0) = x[0]; matrix(0, 1) = x[1]; matrix(0, 2) = x[2];
    matrix(1, 0) = y[0]; matrix(1, 1) = y[1]; matrix(1, 2) = y[2];
    matrix(2, 0) = z[0]; matrix(2, 1) = z[1]; matrix(2, 2) = z[2];
}

} // anonymous namespace

/// @brief     计算局部轨道系相对输入系的角速度（分量在输入系下分解）
/// @param     posInFrame 位置向量
/// @param     velInFrame 速度向量
/// @param     accInFrame 加速度向量，取零时退化为二体（开普勒）情形
/// @return    角速度向量
/// @note      VVLH、LVLH 等局部轨道系彼此只差固定的轴重排，相对输入系的角速度相同，故共用本函数。
///            要求 pos/vel/acc 是同一参考系下位置的一、二阶时间导数。
static Vector3d localOrbitFrameRate(const Vector3d& posInFrame, const Vector3d& velInFrame, const Vector3d& accInFrame)
{
    /*!
        h = r×v 为轨道角动量，ĥ = h/|h| 为轨道法向。角速度由两项组成：

        - 轨道角速度 ω = (r×v)/|r|²，沿 ĥ，大小为 |h|/|r|²。它是二体（开普勒）运动下的精确值：
          此时轨道面在惯性空间固定（ḣ = r×a = 0），角速度只由位置和速度确定。

        - 轨道面进动项 ĥ×(r×a)/|h|，由加速度的离面分量决定。
          因为 ḣ = r×a 且 ĥ·(r×a) = 0，故 dĥ/dt = (r×a)/|h|；单位轴的转动满足 ė = ω×e，而 (ĥ×d)×ĥ = d（d ⊥ ĥ），即得该项。
          取 a 为二体加速度时 r×a = 0，该项消失。

        该项即 J2 交点进动等的来源，量级约 |a|/|v|
    */
    Vector3d h = posInFrame.cross(velInFrame);
    Vector3d rate = h * (1.0 / posInFrame.squaredNorm());
    rate += h.cross(posInFrame.cross(accInFrame)) * (1.0 / h.squaredNorm());
    return rate;
}

errc_t aFrameToVVLHMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix)
{
    Vector3d axis_y = velInFrame.cross(posInFrame);

	// pos为0，vel为0，或者pos和vel方向相同
	if (A_UNLIKELY(normalize(axis_y) == 0))
	{
		matrix.setIdentity();
        return eErrorInvalidParam;
	}
	else
	{
		Vector3d axis_x = posInFrame.cross(axis_y).normalized();
        Vector3d axis_z = -posInFrame.normalized();
		setMatrixRows(matrix, axis_x, axis_y, axis_z);
		return eNoError;
	}
}

errc_t aFrameToVVLHTransform(const Vector3d& posInFrame, const Vector3d& velInFrame, Rotation& rotation)
{
    return aFrameToVVLHMatrix(posInFrame, velInFrame, rotation.getMatrix());
}

errc_t aFrameToVVLHTransform(const Vector3d& posInFrame, const Vector3d& velInFrame, const Vector3d& accInFrame, KinematicRotation& rotation)
{
    errc_t rc = aFrameToVVLHMatrix(posInFrame, velInFrame, rotation.getRotation().getMatrix());
    if (A_UNLIKELY(rc != eNoError))
    {
        rotation.setRotationRate(Vector3d::Zero());
        return rc;
    }

    // VVLH 与 LVLH 只差一组固定的轴重排（X_VVLH = Y_LVLH，Y_VVLH = -Z_LVLH，Z_VVLH = -X_LVLH），
    // 两系相对彼此静止，故相对输入系的角速度相同，公式见 localOrbitFrameRate。
    rotation.setRotationRate(localOrbitFrameRate(posInFrame, velInFrame, accInFrame));
    return eNoError;
}

errc_t aVVLHToFrameMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix)
{
    errc_t rc = aFrameToVVLHMatrix(posInFrame, velInFrame, matrix);
    matrix.transposeInPlace();
    return rc;
}

errc_t aFrameToLVLHMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix)
{
    Vector3d axis_z = posInFrame.cross(velInFrame);
    if(A_UNLIKELY(normalize(axis_z) == 0))
    {
        matrix.setIdentity();
        return eErrorInvalidParam;
    }
    else
    {
        Vector3d axis_y = axis_z.cross(posInFrame).normalized();
        Vector3d axis_x = posInFrame.normalized();
        setMatrixRows(matrix, axis_x, axis_y, axis_z);
        return eNoError;
    }
}

errc_t aFrameToLVLHTransform(const Vector3d& posInFrame, const Vector3d& velInFrame, Rotation& rotation)
{
    return aFrameToLVLHMatrix(posInFrame, velInFrame, rotation.getMatrix());
}

errc_t aFrameToLVLHTransform(const Vector3d& posInFrame, const Vector3d& velInFrame, const Vector3d& accInFrame, KinematicRotation& rotation)
{
    errc_t rc = aFrameToLVLHMatrix(posInFrame, velInFrame, rotation.getRotation().getMatrix());
    if (A_UNLIKELY(rc != eNoError))
    {
        rotation.setRotationRate(Vector3d::Zero());
        return rc;
    }

    // LVLH 是局部轨道系的基准取向，角速度公式见 localOrbitFrameRate。
    rotation.setRotationRate(localOrbitFrameRate(posInFrame, velInFrame, accInFrame));
    return eNoError;
}

errc_t aLVLHToFrameMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix)
{
    errc_t rc = aFrameToLVLHMatrix(posInFrame, velInFrame, matrix);
    matrix.transposeInPlace();
    return rc;
}


errc_t aFrameToVNCMatrix(const Vector3d &posInFrame, const Vector3d &velInFrame, Matrix3d &matrix)
{
    Vector3d axis_y = posInFrame.cross(velInFrame);
    if(A_UNLIKELY(normalize(axis_y) == 0))
    {
        matrix.setIdentity();
        return eErrorInvalidParam;
    }else{
        Vector3d axis_z = velInFrame.cross(axis_y).normalized();
        Vector3d axis_x = velInFrame.normalized();
        setMatrixRows(matrix, axis_x, axis_y, axis_z);
        return eNoError;
    }
}

errc_t aFrameToVNCTransform(const Vector3d &posInFrame, const Vector3d &velInFrame, Rotation &rotation)
{
    return aFrameToVNCMatrix(posInFrame, velInFrame, rotation.getMatrix());
}

errc_t aVNCToFrameMatrix(const Vector3d &posInFrame, const Vector3d &velInFrame, Matrix3d &matrix)
{
    errc_t rc = aFrameToVNCMatrix(posInFrame, velInFrame, matrix);
    matrix.transposeInPlace();
    return rc;
}

errc_t aVNCToFrameTransform(const Vector3d &posInFrame, const Vector3d &velInFrame, Rotation &rotation)
{
    return aVNCToFrameMatrix(posInFrame, velInFrame, rotation.getMatrix());
}

AST_NAMESPACE_END

