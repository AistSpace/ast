///
/// @file      AgentMcpToolAdapter.hpp
/// @brief     智能体工具适配器
/// @details   将AstAI的默认智能体工具注册到通用的MCP服务器
/// @author    axel
/// @date      2026-10-21
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

AST_NAMESPACE_BEGIN

class McpServer;

/*!
    @addtogroup MCP
    @{
*/

/// @brief 将AstAI的默认智能体工具注册到MCP服务器
/// @param server MCP服务器
/// @details 工具的增删由AstAI维护；本函数只做格式转换和转发，不包含具体业务逻辑。
AST_AI_API void aAddAgentToolsToMcpServer(McpServer& server);

/*! @} */

AST_NAMESPACE_END
