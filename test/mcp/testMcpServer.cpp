///
/// @file      testMcpServer.cpp
/// @brief     MCP服务器单元测试
/// @details   测试JSON-RPC消息分派、协议版本协商以及场景工具调用
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

#include "ast/McpServer.hpp"
#include "ast/AgentMcpToolAdapter.hpp"
#include "ast/JsonValue.hpp"
#include "ast/RTTIAPI.hpp"
#include "ast/Test.h"

#include <set>
#include <string>

AST_USING_NAMESPACE

namespace {

/// @brief 发送一行消息并解析响应
JsonValue aParseResponse(McpServer& server, StringView line)
{
    std::string output = server.handleLine(line);
    EXPECT_FALSE(output.empty());
    JsonValue response;
    EXPECT_EQ(response.parseFromString(output), 0);
    return response;
}

/// @brief 从 tools/call 结果中提取文本并解析为JSON
JsonValue aParseToolText(const JsonValue& response)
{
    const JsonValue& content = response["result"]["content"];
    EXPECT_TRUE(content.isArray());
    JsonValue text;
    EXPECT_EQ(text.parseFromString(content[0]["text"].toString()), 0);
    return text;
}

} // namespace


TEST(McpServerTest, Initialize)
{
    McpServer server;
    server.setServerName("TestServer");
    server.setServerVersion("9.9.9");
    server.setInstructions("test instructions");

    JsonValue response = aParseResponse(server, R"({
        "jsonrpc": "2.0",
        "id": 1,
        "method": "initialize",
        "params": {
            "protocolVersion": "2025-11-25",
            "capabilities": {},
            "clientInfo": {"name": "test", "version": "0"}
        }
    })");

    EXPECT_EQ(response["id"].toInt(), 1);
    EXPECT_EQ(response["result"]["protocolVersion"].toString(), "2025-11-25");
    EXPECT_EQ(response["result"]["serverInfo"]["name"].toString(), "TestServer");
    EXPECT_EQ(response["result"]["serverInfo"]["version"].toString(), "9.9.9");
    EXPECT_TRUE(response["result"]["capabilities"]["tools"].isObject());
    EXPECT_EQ(response["result"]["instructions"].toString(), "test instructions");

    // 支持的旧版本原样回显
    response = aParseResponse(server,
        R"({"jsonrpc":"2.0","id":2,"method":"initialize","params":{"protocolVersion":"2025-06-18"}})");
    EXPECT_EQ(response["result"]["protocolVersion"].toString(), "2025-06-18");

    // 不支持的版本回退到默认版本
    response = aParseResponse(server,
        R"({"jsonrpc":"2.0","id":3,"method":"initialize","params":{"protocolVersion":"1999-01-01"}})");
    EXPECT_EQ(response["result"]["protocolVersion"].toString(), "2025-11-25");
}

TEST(McpServerTest, NotificationsAndPing)
{
    McpServer server;

    // 通知不需要回包，未知通知同样不回包
    EXPECT_TRUE(server.handleLine(
        R"({"jsonrpc":"2.0","method":"notifications/initialized"})").empty());
    EXPECT_TRUE(server.handleLine(
        R"({"jsonrpc":"2.0","method":"notifications/cancelled","params":{"requestId":1}})").empty());
    EXPECT_TRUE(server.handleLine(
        R"({"jsonrpc":"2.0","method":"some/unknown-notification"})").empty());

    // 空行忽略
    EXPECT_TRUE(server.handleLine("   ").empty());

    // ping 原样回显字符串id
    JsonValue response = aParseResponse(server,
        R"({"jsonrpc":"2.0","id":"abc","method":"ping"})");
    EXPECT_EQ(response["id"].toString(), "abc");
    EXPECT_TRUE(response["result"].isObject());
}

TEST(McpServerTest, ToolsList)
{
    McpServer server;
    aAddAgentToolsToMcpServer(server);
    aAddAgentToolsToMcpServer(server);   // 重复调用会重新初始化工具集合，不会累积
    EXPECT_EQ(server.toolCount(), 5u);

    JsonValue response = aParseResponse(server,
        R"({"jsonrpc":"2.0","id":10,"method":"tools/list"})");

    const JsonValue& toolsValue = response["result"]["tools"];
    ASSERT_TRUE(toolsValue.isArray());
    const std::vector<JsonValue>& tools = toolsValue.getArray();
    ASSERT_EQ(tools.size(), 5u);

    std::set<std::string> names;
    for(const auto& tool : tools)
    {
        names.insert(tool["name"].toString());
        EXPECT_TRUE(tool["inputSchema"].isObject());
        EXPECT_EQ(tool["inputSchema"]["type"].toString(), "object");
        EXPECT_TRUE(tool["function"].isNull());   // 不是OpenAI工具格式
    }

    EXPECT_EQ(names.count("find_classes"), 1u);
    EXPECT_EQ(names.count("create_object"), 1u);
    EXPECT_EQ(names.count("find_objects"), 1u);
    EXPECT_EQ(names.count("get_object_attributes"), 1u);
    EXPECT_EQ(names.count("set_object_attribute"), 1u);
}

TEST(McpServerTest, ToolCallFindClasses)
{
    McpServer server;
    aAddAgentToolsToMcpServer(server);

    JsonValue response = aParseResponse(server, R"({
        "jsonrpc": "2.0",
        "id": 11,
        "method": "tools/call",
        "params": {"name": "find_classes", "arguments": {}}
    })");

    ASSERT_TRUE(response["result"].isObject());
    EXPECT_FALSE(response["result"]["isError"].toBool(true));

    JsonValue classes = aParseToolText(response);
    ASSERT_TRUE(classes.isArray());

    bool foundSatellite = false;
    for(const auto& cls : classes.getArray())
    {
        if(cls["name"].toString() == "Satellite")
        {
            foundSatellite = true;
            break;
        }
    }
    EXPECT_TRUE(foundSatellite);
}

TEST(McpServerTest, CreateAndModifyObject)
{
    aRemoveAllObjects();
    McpServer server;
    aAddAgentToolsToMcpServer(server);

    // 创建对象
    JsonValue response = aParseResponse(server, R"({
        "jsonrpc": "2.0",
        "id": 20,
        "method": "tools/call",
        "params": {
            "name": "create_object",
            "arguments": {"class": "Satellite", "name": "mcp_test"}
        }
    })");
    ASSERT_TRUE(response["result"].isObject());
    EXPECT_FALSE(response["result"]["isError"].toBool(true));

    JsonValue object = aParseToolText(response);
    ASSERT_TRUE(object.isObject());
    EXPECT_EQ(object["name"].toString(), "mcp_test");
    EXPECT_EQ(object["_type"].toString(), "Satellite");
    EXPECT_TRUE(object["_id"].isNumber());
    const int id = object["_id"].toInt();

    // 查找对象，参数缺省也可以
    response = aParseResponse(server, R"({
        "jsonrpc": "2.0",
        "id": 21,
        "method": "tools/call",
        "params": {"name": "find_objects"}
    })");
    JsonValue objects = aParseToolText(response);
    ASSERT_TRUE(objects.isArray());
    bool found = false;
    for(const auto& item : objects.getArray())
    {
        if(item["_id"].toInt() == id)
        {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);

    // 修改属性
    JsonValue request;
    request["jsonrpc"] = "2.0";
    request["id"] = 22;
    request["method"] = "tools/call";
    request["params"]["name"] = "set_object_attribute";
    request["params"]["arguments"]["id"] = id;
    request["params"]["arguments"]["attribute"] = "name";
    request["params"]["arguments"]["value"] = "mcp_renamed";
    response = aParseResponse(server, request.toJsonString(0));
    ASSERT_TRUE(response["result"].isObject());
    EXPECT_FALSE(response["result"]["isError"].toBool(true));

    // 读取属性
    request = JsonValue();
    request["jsonrpc"] = "2.0";
    request["id"] = 23;
    request["method"] = "tools/call";
    request["params"]["name"] = "get_object_attributes";
    request["params"]["arguments"]["id"] = id;
    response = aParseResponse(server, request.toJsonString(0));

    JsonValue attributes = aParseToolText(response);
    ASSERT_TRUE(attributes.isObject());
    EXPECT_EQ(attributes["name"].toString(), "mcp_renamed");
}

TEST(McpServerTest, GenericTool)
{
    // 服务器本身不依赖任何业务工具：默认为空，可通过 addTool 注册任意函数
    McpServer server;

    JsonValue response = aParseResponse(server,
        R"({"jsonrpc":"2.0","id":50,"method":"tools/list"})");
    ASSERT_TRUE(response["result"]["tools"].isArray());
    EXPECT_EQ(response["result"]["tools"].getArray().size(), 0u);

    McpTool* tool = server.addTool([](const JsonValue& arguments) -> std::string {
        return std::string("echo:") + arguments["text"].toString();
    });
    ASSERT_TRUE(tool != nullptr);
    tool->setName("echo");
    tool->setDescription("Echo text");

    JsonValue schema;
    schema["type"] = "object";
    schema["properties"]["text"]["type"] = "string";
    tool->setInputSchema(schema);

    EXPECT_EQ(server.toolCount(), 1u);
    EXPECT_TRUE(server.findTool("echo") == tool);

    response = aParseResponse(server,
        R"({"jsonrpc":"2.0","id":51,"method":"tools/call","params":{"name":"echo","arguments":{"text":"hi"}}})");
    ASSERT_TRUE(response["result"].isObject());
    EXPECT_FALSE(response["result"]["isError"].toBool(true));
    EXPECT_EQ(response["result"]["content"][0]["text"].toString(), "echo:hi");

    server.clearTools();
    EXPECT_EQ(server.toolCount(), 0u);
}

TEST(McpServerTest, ProtocolErrors)
{
    McpServer server;
    aAddAgentToolsToMcpServer(server);

    // 解析错误：id为null
    JsonValue response = aParseResponse(server, "{");
    EXPECT_TRUE(response["id"].isNull());
    EXPECT_EQ(response["error"]["code"].toInt(), -32700);

    // 顶层不是对象 / 批量请求
    response = aParseResponse(server, "[]");
    EXPECT_TRUE(response["id"].isNull());
    EXPECT_EQ(response["error"]["code"].toInt(), -32600);

    // 缺少jsonrpc
    response = aParseResponse(server,
        R"({"id":30,"method":"ping"})");
    EXPECT_EQ(response["error"]["code"].toInt(), -32600);

    // 未知方法
    response = aParseResponse(server,
        R"({"jsonrpc":"2.0","id":31,"method":"unknown/method"})");
    EXPECT_EQ(response["error"]["code"].toInt(), -32601);

    // 未知工具
    response = aParseResponse(server,
        R"({"jsonrpc":"2.0","id":32,"method":"tools/call","params":{"name":"no_such_tool","arguments":{}}})");
    EXPECT_EQ(response["error"]["code"].toInt(), -32602);

    // 容忍首行的UTF-8 BOM
    response = aParseResponse(server,
        "\xEF\xBB\xBF{\"jsonrpc\":\"2.0\",\"id\":40,\"method\":\"ping\"}");
    EXPECT_EQ(response["id"].toInt(), 40);

    // arguments 不是对象
    response = aParseResponse(server,
        R"({"jsonrpc":"2.0","id":33,"method":"tools/call","params":{"name":"find_objects","arguments":[]}})");
    EXPECT_EQ(response["error"]["code"].toInt(), -32602);
}

GTEST_MAIN()
