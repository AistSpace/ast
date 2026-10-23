# AppMCP

`AstMCP` 的 stdio 服务器入口。Agent通过[Model Context Protocol](https://modelcontextprotocol.io/) 启动本程序后，
可以直接调用 SpaceAST 的场景工具：查询可用对象类型、创建对象、读取和修改对象属性。

## 接入 Claude Code

```bash
claude mcp add --transport stdio ast -- "<bin>AppMCP.exe"
claude mcp list
```

注意将 `<bin>` 替换为真实文件夹路径

也可以在项目里放一个 `.mcp.json`：

```json
{
  "mcpServers": {
    "ast": {
      "type": "stdio",
      "command": "<bin>/AppMCP.exe",
      "args": []
    }
  }
}
```
注意将 `<bin>` 替换为真实文件夹路径

会话中输入 `/mcp` 可以查看连接状态。


## 注意事项

- 场景只存在于 AppMCP 进程内。客户端断开、进程退出后场景即丢失；每次启动都是空场景。
- stdout 专用于 JSON-RPC 协议。日志统一写 stderr；不要把 `printf`/`std::cout` 加到服务器代码里。
- 目前是单线程、串行处理请求；不支持取消/进度通知。
