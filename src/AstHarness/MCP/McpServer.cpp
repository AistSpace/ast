///
/// @file      McpServer.cpp
/// @brief     MCP服务器实现
/// @details   实现JSON-RPC消息分派、工具调用以及stdio消息循环
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

#include "McpServer.hpp"
#include "McpProtocol.hpp"
#include "AstConfig.h"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/Logger.hpp"

#include <csignal>
#include <cstdio>
#include <exception>
#include <iostream>
#include <map>
#include <string>

#ifdef _WIN32
#    include <fcntl.h>
#    include <io.h>
#endif

AST_NAMESPACE_BEGIN

namespace {

/// @brief 获取对象中的字段，不存在时返回null
/// @note 不能使用 JsonValue 的 const operator[]，它在对象上会插入空值
const JsonValue& aMcpGetField(const JsonValue& value, const char* key)
{
    static const JsonValue nullValue;
    if(!value.isObject())
        return nullValue;
    const std::map<std::string, JsonValue>& object = value.getObject();
    auto it = object.find(key);
    return it == object.end() ? nullValue : it->second;
}

/// @brief 判断请求是否带有id（带id的是请求，不带id的是通知）
bool aMcpHoldsId(const JsonValue& request, JsonValue& id)
{
    if(!request.isObject())
        return false;
    const std::map<std::string, JsonValue>& object = request.getObject();
    auto it = object.find("id");
    if(it == object.end())
        return false;
    id = it->second;
    return true;
}

/// @brief 构造成功响应
JsonValue aMcpMakeSuccess(const JsonValue& id, const JsonValue& result)
{
    JsonValue response;
    response["jsonrpc"] = kMcpJsonRpcVersion;
    response["id"] = id;
    response["result"] = result;
    return response;
}

/// @brief 构造错误响应
JsonValue aMcpMakeError(const JsonValue& id, EMcpErrorCode code, StringView message)
{
    JsonValue error;
    error["code"] = static_cast<int>(code);
    error["message"] = std::string(message);

    JsonValue response;
    response["jsonrpc"] = kMcpJsonRpcVersion;
    response["id"] = id;
    response["error"] = error;
    return response;
}

/// @brief 将工具转换为MCP tools/list 中的描述
JsonValue aMcpToolToJson(const McpTool& tool)
{
    JsonValue json;
    json["name"] = tool.name();

    if(!tool.description().empty())
        json["description"] = tool.description();

    JsonValue schema = tool.inputSchema();
    if(!schema.isObject())
    {
        schema.setObject({});
        schema["type"] = "object";
        schema["properties"].setObject({});
    }
    json["inputSchema"] = schema;

    const JsonValue& annotations = tool.annotations();
    if(annotations.isObject() && annotations.size() > 0)
        json["annotations"] = annotations;

    return json;
}

/// @brief 构造 tools/call 的文本结果
JsonValue aMcpMakeToolResult(StringView text, bool isError)
{
    JsonValue contentItem;
    contentItem["type"] = "text";
    contentItem["text"] = std::string(text);

    JsonValue content;
    content.append(contentItem);

    JsonValue result;
    result["content"] = content;
    result["isError"] = isError;
    return result;
}

} // namespace


McpServer::McpServer()
    : serverName_("McpServer")
    , serverVersion_(AST_VERSION)
{
}

McpServer::~McpServer() = default;

void McpServer::setServerName(StringView name)
{
    serverName_ = std::string(name);
}

void McpServer::setServerVersion(StringView version)
{
    serverVersion_ = std::string(version);
}

void McpServer::setInstructions(StringView instructions)
{
    instructions_ = std::string(instructions);
}

McpTool* McpServer::addTool(std::unique_ptr<McpTool> tool)
{
    return tools_.addTool(std::move(tool));
}

void McpServer::clearTools()
{
    tools_.clearTools();
}

McpTool* McpServer::findTool(StringView name) const
{
    return tools_.findTool(name);
}

bool McpServer::handleRequest(const JsonValue& request, JsonValue& response)
{
    response = JsonValue();

    JsonValue id;
    if(!aMcpHoldsId(request, id))
        return false;   // 通知：不需要回包

    if(!request.isObject())
    {
        response = aMcpMakeError(JsonValue(), EMcpErrorCode::eInvalidRequest, "Invalid Request");
        return true;
    }

    if(aMcpGetField(request, "jsonrpc").toString() != kMcpJsonRpcVersion)
    {
        response = aMcpMakeError(id, EMcpErrorCode::eInvalidRequest,
                                 "Invalid Request: expected jsonrpc \"2.0\"");
        return true;
    }

    const std::string method = aMcpGetField(request, "method").toString();
    if(method.empty())
    {
        response = aMcpMakeError(id, EMcpErrorCode::eInvalidRequest,
                                 "Invalid Request: missing method");
        return true;
    }

    const JsonValue& params = aMcpGetField(request, "params");
    if(method == kMcpMethodInitialize)
    {
        response = aMcpMakeSuccess(id, handleInitialize(params));
    }
    else if(method == kMcpMethodPing)
    {
        JsonValue result;
        result.setObject({});
        response = aMcpMakeSuccess(id, result);
    }
    else if(method == kMcpMethodToolsList)
    {
        response = aMcpMakeSuccess(id, handleToolsList());
    }
    else if(method == kMcpMethodToolsCall)
    {
        JsonValue result;
        int errorCode = 0;
        std::string errorMessage;
        if(handleToolsCall(params, result, errorCode, errorMessage))
            response = aMcpMakeSuccess(id, result);
        else
            response = aMcpMakeError(id, static_cast<EMcpErrorCode>(errorCode), errorMessage);
    }
    else
    {
        response = aMcpMakeError(id, EMcpErrorCode::eMethodNotFound,
                                 std::string("Method not found: ") + method);
    }
    return true;
}

std::string McpServer::handleLine(StringView line)
{
    // 容忍首行的UTF-8 BOM（部分管道/客户端会自动附加）
    if(line.size() >= 3
       && static_cast<unsigned char>(line[0]) == 0xEF
       && static_cast<unsigned char>(line[1]) == 0xBB
       && static_cast<unsigned char>(line[2]) == 0xBF)
    {
        line.remove_prefix(3);
    }

    // 去掉行尾空白（Windows文本模式下可能残留'\r'）
    while(!line.empty())
    {
        const char c = line.back();
        if(c == '\r' || c == '\n' || c == ' ' || c == '\t')
            line.remove_suffix(1);
        else
            break;
    }
    if(line.empty())
        return std::string();

    JsonValue request;
    if(request.parseFromString(line) != 0)
    {
        JsonValue error = aMcpMakeError(JsonValue(), EMcpErrorCode::eParseError, "Parse error");
        return error.toJsonString(0);
    }

    // 顶层必须是对象；MCP 2025-06-18 起不再支持JSON-RPC批量请求
    if(!request.isObject())
    {
        JsonValue error = aMcpMakeError(JsonValue(), EMcpErrorCode::eInvalidRequest,
                                        request.isArray()
                                            ? "Invalid Request: batch requests are not supported"
                                            : "Invalid Request: expected an object");
        return error.toJsonString(0);
    }

    JsonValue response;
    if(!handleRequest(request, response))
        return std::string();
    return response.toJsonString(0);
}

int McpServer::runStdio()
{
#ifdef _WIN32
    // stdout用于协议输出，必须使用二进制模式，避免'\n'被转换为"\r\n"
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#else
    std::signal(SIGPIPE, SIG_IGN);
#endif

    std::ios::sync_with_stdio(false);

    std::string line;
    while(std::getline(std::cin, line))
    {
        std::string output = handleLine(line);
        if(output.empty())
            continue;
        std::cout << output << '\n';
        std::cout.flush();
        if(!std::cout)
            break;   // 客户端已关闭输出管道
    }
    return 0;
}

JsonValue McpServer::handleInitialize(const JsonValue& params) const
{
    const std::string requestedVersion = aMcpGetField(params, "protocolVersion").toString();
    const std::string version = aMcpIsSupportedProtocolVersion(requestedVersion)
                                    ? requestedVersion
                                    : std::string(kMcpDefaultProtocolVersion);
    if(!requestedVersion.empty() && version != requestedVersion)
        aWarning(_("客户端请求的MCP协议版本 '%s' 不受支持，使用 '%s'"),
                 requestedVersion.c_str(), version.c_str());

    JsonValue result;
    result["protocolVersion"] = version;

    JsonValue capabilities;
    capabilities["tools"].setObject({});
    result["capabilities"] = capabilities;

    JsonValue serverInfo;
    serverInfo["name"] = serverName_;
    serverInfo["version"] = serverVersion_;
    result["serverInfo"] = serverInfo;

    if(!instructions_.empty())
        result["instructions"] = instructions_;

    return result;
}

JsonValue McpServer::handleToolsList() const
{
    JsonValue toolList;
    toolList.setArray({});
    const std::vector<std::unique_ptr<McpTool>>& tools = tools_.tools();
    for(size_t i = 0; i < tools.size(); ++i)
    {
        if(!tools[i] || tools[i]->name().empty())
            continue;   // 未命名的工具不对外暴露
        toolList.append(aMcpToolToJson(*tools[i]));
    }

    JsonValue result;
    result["tools"] = toolList;
    return result;
}

bool McpServer::handleToolsCall(const JsonValue& params, JsonValue& result,
                                int& errorCode, std::string& errorMessage) const
{
    const std::string name = aMcpGetField(params, "name").toString();
    McpTool* tool = findTool(name);
    if(!tool)
    {
        errorCode = static_cast<int>(EMcpErrorCode::eInvalidParams);
        errorMessage = name.empty() ? std::string("Invalid params: missing tool name")
                                    : std::string("Unknown tool: ") + name;
        return false;
    }

    JsonValue arguments = aMcpGetField(params, "arguments");
    if(arguments.isNull())
        arguments.setObject({});
    else if(!arguments.isObject())
    {
        errorCode = static_cast<int>(EMcpErrorCode::eInvalidParams);
        errorMessage = "Invalid params: arguments must be an object";
        return false;
    }

    try
    {
        result = aMcpMakeToolResult(tool->call(arguments), false);
    }
    catch(const std::exception& e)
    {
        result = aMcpMakeToolResult(e.what(), true);
    }
    catch(...)
    {
        result = aMcpMakeToolResult("Unknown error while calling tool", true);
    }
    return true;
}

AST_NAMESPACE_END
