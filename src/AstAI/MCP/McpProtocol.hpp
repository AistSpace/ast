///
/// @file      McpProtocol.hpp
/// @brief     MCP协议常量
/// @details   定义JSON-RPC错误码、方法名以及服务器支持的MCP协议版本
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
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup MCP
    @brief Model Context Protocol 服务器接口
    @{
*/

/// @brief JSON-RPC错误码
enum class EMcpErrorCode : int
{
    eParseError     = -32700,   ///< 解析错误
    eInvalidRequest = -32600,   ///< 无效请求
    eMethodNotFound = -32601,   ///< 方法不存在
    eInvalidParams  = -32602,   ///< 参数无效
    eInternalError  = -32603,   ///< 内部错误
};

/// @brief JSON-RPC版本
constexpr const char* kMcpJsonRpcVersion = "2.0";

/// @brief 默认（最高支持）协议版本
constexpr const char* kMcpProtocolVersion20251125 = "2025-11-25";
/// @brief 兼容的协议版本
constexpr const char* kMcpProtocolVersion20250618 = "2025-06-18";
constexpr const char* kMcpProtocolVersion20250326 = "2025-03-26";
constexpr const char* kMcpProtocolVersion20241105 = "2024-11-05";
/// @brief 默认协议版本
constexpr const char* kMcpDefaultProtocolVersion = kMcpProtocolVersion20251125;

/// @brief 方法名
constexpr const char* kMcpMethodInitialize             = "initialize";
constexpr const char* kMcpMethodInitialized            = "notifications/initialized";
constexpr const char* kMcpMethodCancelled              = "notifications/cancelled";
constexpr const char* kMcpMethodPing                   = "ping";
constexpr const char* kMcpMethodToolsList              = "tools/list";
constexpr const char* kMcpMethodToolsCall              = "tools/call";

/// @brief 判断协议版本是否受支持
/// @param version 客户端请求的协议版本
/// @return 是否受支持
inline bool aMcpIsSupportedProtocolVersion(StringView version)
{
    return version == kMcpProtocolVersion20251125
        || version == kMcpProtocolVersion20250618
        || version == kMcpProtocolVersion20250326
        || version == kMcpProtocolVersion20241105;
}

/*! @} */

AST_NAMESPACE_END
