///
/// @file      Stepper.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/Interval.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// 步进器基类
/// @details 
/// 采样点由步进器自身决定（不保证起点恰为区间起点）
/// 先调用 first() 取第一个采样点，再用 next() 依次取后续采样点
class AST_CORE_API Stepper
{
public:
    Stepper() = default;
    virtual ~Stepper() = default;

    /// 初始化步进器
    void init(const Interval& interval){init(interval.start(), interval.stop());}

    /// 初始化步进器
    virtual void init(double start, double end) = 0;

    /// 获取第一个采样点
    /// @param[out] t 第一个采样点时刻
    /// @return 有采样点时返回 true
    virtual bool first(double& t) = 0;

    /// 获取下一个采样点
    /// @param[out] t 采样点时刻
    /// @return 还有采样点时返回 true
    virtual bool next(double& t) = 0;
};

/*! @} */

AST_NAMESPACE_END
