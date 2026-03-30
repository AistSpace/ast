///
/// @file      AxesFrozen.cpp
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

#include "AxesFrozen.hpp"
#include "AstMath/KinematicRotation.hpp"

AST_NAMESPACE_BEGIN

PAxesFrozen AxesFrozen::New(Axes* axes, const TimePoint& epoch, Axes* referenceAxes)
{
    return new AxesFrozen(axes, epoch, referenceAxes);
}

HAxesFrozen AxesFrozen::MakeShared(Axes *axes, const TimePoint &epoch, Axes *referenceAxes)
{
    return new AxesFrozen(axes, epoch, referenceAxes);
}

AxesFrozen::AxesFrozen(Axes *axes, const TimePoint &epoch, Axes *referenceAxes)
    : referenceAxes_(referenceAxes)
    , axes_(axes)
    , epoch_(epoch)
{
}

Axes *AxesFrozen::getParent() const
{
    return referenceAxes_;
}

errc_t AxesFrozen::getTransform(const TimePoint &tp, Rotation &rotation) const
{
    A_UNUSED(tp);
    auto epoch = getEpoch();
    return aAxesTransform(referenceAxes_, axes_, epoch, rotation);
}

errc_t AxesFrozen::getTransform(const TimePoint &tp, KinematicRotation &rotation) const
{
    A_UNUSED(tp);
    auto epoch = getEpoch();
    rotation.setRotationRate(Vector3d::Zero());
    return aAxesTransform(referenceAxes_, axes_, epoch, rotation.getRotation());
}

AST_NAMESPACE_END
