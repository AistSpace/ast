#!/bin/sh
# AstAgent：航天任务设计智能体

target "AstAgent"
    set_kind "shared"
    add_files "**.cpp"
    add_includedirs "../../include/AstAgent" "{public}"
    add_defines "AST_BUILD_LIB_AGENT"
    add_deps "AstHarness" "AstUtil" "AstCore"
    add_headerfiles "(**.hpp)" "AstAgent"
    add_headerfiles "(**.h)" "AstAgent"
target_end
