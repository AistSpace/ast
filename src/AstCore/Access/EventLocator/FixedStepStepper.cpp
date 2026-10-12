///
/// @file      FixedStepStepper.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-11
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

#include "FixedStepStepper.hpp"

AST_NAMESPACE_BEGIN

FixedStepStepper::FixedStepStepper(double stepSize)
    : stepSize_(stepSize)
{
}

void FixedStepStepper::init(double start, double end)
{
    start_   = start;
    end_    = end;
    current_ = start;
}


bool FixedStepStepper::first(double& t)
{
    current_ = start_;
    return FixedStepStepper::next(t);
}


bool FixedStepStepper::next(double& t)
{
    if (current_ > end_) { 
        t = end_;
        return false; 
    }

    t = current_;
    current_ = current_ + stepSize_;
    return true;
}

AST_NAMESPACE_END
