#!/bin/sh
# AstHarness：AI 运行时（模型接入、工具调用循环、会话、MCP）

target "AstHarness"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstHarness" "{public}"
    add_defines "AST_BUILD_LIB_HARNESS"
    add_deps "AstUtil"
    add_headerfiles "(**.hpp)" "AstHarness"
    add_headerfiles "(**.h)" "AstHarness"
target_end
