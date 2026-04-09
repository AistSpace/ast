///
/// @file      EventTimeFallback.hpp
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
#include "AstCore/EventTime.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief  支持fallback的事件时间
/// @details 当第一个EventTime计算失败时，使用第二个EventTime的值
class AST_CORE_API EventTimeFallback : public EventTime
{
public:
    static EventTimeFallback* New(EventTime* primary, EventTime* fallback);
    static EventTimeFallback* New(EventTime* primary, const TimePoint& fallback);
    static SharedPtr<EventTimeFallback> MakeShared(EventTime* primary, EventTime* fallback);
    static SharedPtr<EventTimeFallback> MakeShared(EventTime* primary, const TimePoint& fallback);


    EventTimeFallback() = default;
    EventTimeFallback(EventTime* primary, EventTime* fallback);
    EventTimeFallback(EventTime* primary, const TimePoint& fallback);

    ~EventTimeFallback() override = default;

    errc_t getTime(TimePoint& tp) const override;
    void setPrimary(EventTime* primary);
    void setFallback(EventTime* fallback);
protected:
    SharedPtr<EventTime> primary_{};       ///< 主要事件时间
    SharedPtr<EventTime> fallback_{};      ///< 后备事件时间
};

/*! @} */

AST_NAMESPACE_END