///
/// @file      AxesGTOD.cpp
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

#include "AxesGTOD.hpp"
#include "AxesTOD.hpp"
#include "AstCore/EarthFrame.hpp"

AST_NAMESPACE_BEGIN

AxesGTOD *AxesGTOD::Instance()
{
    static SharedPtr<AxesGTOD> instance(new AxesGTOD());
    return instance.get();
}

Axes *AxesGTOD::getParent() const
{
    return AxesTOD::Instance();
}

err_t AxesGTOD::getTransform(const TimePoint &tp, Rotation &rotation) const
{
    aTODToGTODTransform(tp, rotation);
    return eNoError;
}

err_t AxesGTOD::getTransform(const TimePoint &tp, KinematicRotation &rotation) const
{
    aTODToGTODTransform(tp, rotation);
    return eNoError;
}

AST_NAMESPACE_END

