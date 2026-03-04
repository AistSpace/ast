///
/// @file      AxesTOD.cpp
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

#include "AxesTOD.hpp"
#include "AxesMOD.hpp"
#include "AstCore/EarthFrame.hpp"
#include "AstMath/KinematicRotation.hpp"

AST_NAMESPACE_BEGIN


AxesTOD *AxesTOD::Instance()
{
    static SharedPtr<AxesTOD> instance(new AxesTOD());
    return instance.get();
}


Axes *AxesTOD::getParent() const
{
    return AxesMOD::Instance();
}

err_t AxesTOD::getTransform(const TimePoint &tp, Rotation &rotation) const
{
    aMODToTODTransform(tp, rotation);
    return eNoError;
}

err_t AxesTOD::getTransform(const TimePoint &tp, KinematicRotation &rotation) const
{
    aMODToTODTransform(tp, rotation.getRotation());
    rotation.setRotationRate(Vector3d::Zero());
    return eNoError;
}

AST_NAMESPACE_END

