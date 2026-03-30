///
/// @file      EventIntervalLinkTo.cpp
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

#include "EventIntervalLinkTo.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

EventIntervalLinkTo *EventIntervalLinkTo::New(StringView name, StringView absolutePath)
{
    return new EventIntervalLinkTo(name, absolutePath);
}

EventIntervalLinkTo::EventIntervalLinkTo(StringView name, StringView absolutePath)
    : name_(name)
    , absolutePath_(absolutePath)
{
}

errc_t EventIntervalLinkTo::getInterval(TimeInterval &interval) const
{
    // @todo 实现链接到其他时间区间的逻辑
    return -1;
}

AST_NAMESPACE_END

