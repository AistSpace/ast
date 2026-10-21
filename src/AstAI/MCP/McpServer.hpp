///
/// @file      McpServer.hpp
/// @brief     MCP服务器
/// @details   实现基于stdio的Model Context Protocol服务器
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
#include "McpTool.hpp"
#include "AstAI/ChatTools.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/StringView.hpp"
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief MCP服务器
/// @details 按行收发JSON-RPC消息，通过stdio与MCP客户端通信。
///          服务器只负责协议处理，不包含任何具体业务工具；工具通过 addTool 注册。
class AST_AI_API McpServer
{
public:
    McpServer();
    ~McpServer();

    /// @brief 设置服务器名称（initialize响应中的 serverInfo.name）
    void setServerName(StringView name);
    const std::string& serverName() const { return serverName_; }

    /// @brief 设置服务器版本（initialize响应中的 serverInfo.version）
    void setServerVersion(StringView version);
    const std::string& serverVersion() const { return serverVersion_; }

    /// @brief 设置使用说明（initialize响应中的 instructions）
    void setInstructions(StringView instructions);
    const std::string& instructions() const { return instructions_; }

    /// @brief 获取工具集合，可直接向其中注册工具
    ChatTools& tools() { return tools_; }
    const ChatTools& tools() const { return tools_; }

    /// @brief 添加工具
    /// @param tool 工具指针
    /// @return 添加成功返回工具指针；同名工具已存在时返回nullptr
    /// @note 工具名称可以在添加后通过 McpTool::setName 设置
    McpTool* addTool(std::unique_ptr<McpTool> tool);

    /// @brief 添加派生类形式的工具
    template<typename T>
    McpTool* addTool(std::unique_ptr<T> tool)
    {
        static_assert(std::is_base_of<McpTool, T>::value, "T must derive from McpTool");
        return addTool(std::unique_ptr<McpTool>(tool.release()));
    }

    /// @brief 添加函数形式的工具
    /// @param func 工具函数，签名为 std::string(const JsonValue&)
    /// @return 添加成功返回工具指针；同名工具已存在时返回nullptr
    template<typename Func>
    McpTool* addTool(Func func)
    {
        return addTool(std::unique_ptr<McpTool>(new McpToolGeneric<Func>(std::move(func))));
    }

    /// @brief 清除所有工具
    void clearTools();

    /// @brief 获取工具数量
    size_t toolCount() const { return tools_.size(); }

    /// @brief 查找工具
    /// @param name 工具名称
    /// @return 工具指针，不存在返回nullptr
    McpTool* findTool(StringView name) const;

    /// @brief 处理一条JSON-RPC消息
    /// @param request 请求
    /// @param[out] response 响应
    /// @return 是否需要回包（通知不需要回包）
    bool handleRequest(const JsonValue& request, JsonValue& response);

    /// @brief 处理一行文本消息
    /// @param line 一行JSON文本
    /// @return 响应文本（通知、空行返回空字符串）
    std::string handleLine(StringView line);

    /// @brief 在stdio上运行服务器主循环
    /// @return 进程退出码
    int runStdio();

private:
    JsonValue handleInitialize(const JsonValue& params) const;
    JsonValue handleToolsList() const;
    bool handleToolsCall(const JsonValue& params, JsonValue& result,
                         int& errorCode, std::string& errorMessage) const;

    std::string serverName_{};                  ///< 服务器名称
    std::string serverVersion_{};               ///< 服务器版本
    std::string instructions_{};                ///< 使用说明
    ChatTools tools_{};                         ///< 工具集合

    A_DISABLE_COPY(McpServer)
};

/*! @} */

AST_NAMESPACE_END
