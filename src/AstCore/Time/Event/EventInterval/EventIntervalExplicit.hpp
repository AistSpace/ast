///
/// @file      EventIntervalExplicit.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/EventInterval.hpp"
#include "AstCore/TimeInterval.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class EventIntervalExplicit;
using PEventIntervalExplicit = EventIntervalExplicit*;
using HEventIntervalExplicit = SharedPtr<EventIntervalExplicit>;

class AST_CORE_API EventIntervalExplicit final: public EventInterval
{
public:
    static PEventIntervalExplicit New(const TimeInterval& interval);
    static PEventIntervalExplicit New(const TimePoint& startTime, const TimePoint& stopTime);
    static HEventIntervalExplicit MakeShared(const TimeInterval& interval);
    
    EventIntervalExplicit() = default;
    EventIntervalExplicit(const TimeInterval& interval);
    ~EventIntervalExplicit() = default;
    err_t getInterval(TimeInterval& interval) const override;
protected:
    TimeInterval interval_;     ///< 事件时间段
};

/*! @} */

AST_NAMESPACE_END
