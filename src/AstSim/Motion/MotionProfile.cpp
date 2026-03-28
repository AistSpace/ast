///
/// @file      MotionProfile.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
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

#include "MotionProfile.hpp"
#include "MotionTwoBody.hpp"
#include "MotionHPOP.hpp"
#include "MotionSPICE.hpp"

AST_NAMESPACE_BEGIN


MotionHPOP *MotionProfile::toHPOP() const
{
    /// @todo 提升类型动态转换效率
    return dynamic_cast<MotionHPOP*>(const_cast<MotionProfile*>(this));
}

MotionTwoBody *MotionProfile::toTwoBody() const
{
    /// @todo 提升类型动态转换效率
    return dynamic_cast<MotionTwoBody*>(const_cast<MotionProfile*>(this));
}

MotionSPICE *MotionProfile::toSPICE() const
{
    /// @todo 提升类型动态转换效率
    return dynamic_cast<MotionSPICE*>(const_cast<MotionProfile*>(this));
}

AST_NAMESPACE_END

