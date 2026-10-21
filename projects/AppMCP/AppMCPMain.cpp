///
/// @file      AppMCPMain.cpp
/// @brief     MCP服务器程序入口
/// @details   通过stdio与MCP客户端通信
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

#include "AstAgent/AgentMcpToolAdapter.hpp"
#include "AstHarness/McpServer.hpp"
#include "AstSim/Satellite.hpp"

AST_USING_NAMESPACE

int main()
{
    {Satellite satellite;}  // 确保链接到AstSim库，使其中的对象类型注册到RTTI系统

    // 注意：stdout专用于MCP协议消息，任何调试信息都必须输出到stderr
    McpServer server;
    server.setServerName("AstMCP");
    server.setInstructions(u8"Ast 航天任务场景工具。对象只存在于本服务器进程内，进程退出后场景丢失。");

    // 注册AstAgent提供的场景工具（具体工具由AgentInit维护）
    aAddAgentToolsToMcpServer(server);

    return server.runStdio();
}
