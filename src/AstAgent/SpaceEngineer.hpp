///
/// @file      SpaceEngineer.hpp
/// @brief     航天工程师智能体
/// @details   定义面向航天任务设计与分析的默认智能体配置。
/// @author    axel
/// @date      2026-10-22
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
#include "AstHarness/AssistantAgent.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

/// @brief 创建航天工程师Agent（注册默认航天工具和系统提示词）
AST_AGENT_API AssistantAgent aSpaceEngineerAgent();

/// @brief 创建航天工程师Agent（注册默认航天工具和系统提示词）
AST_AGENT_API AssistantAgent* aNewSpaceEngineerAgent();

/*! @} */

AST_NAMESPACE_END
