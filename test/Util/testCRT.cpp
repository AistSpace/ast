///
/// @file      testCRT.cpp
/// @brief     CRT模块单元测试
/// @details   测试命令行参数获取等功能
/// @author    axel
/// @date      2026-04-15
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。

#include "AstUtil/CRT.hpp"
#include "AstTest/Test.h"
#include <cstdio>

AST_USING_NAMESPACE

// 测试获取命令行参数
TEST(CRT, ArgsGet)
{
    args_t args = aArgsGet();
    
    printf("argc: %d\n", args.argc);
    printf("argv[0]: %s\n", args.argv[0] ? args.argv[0] : "null");
    
    // argc 应该大于 0
    EXPECT_GT(args.argc, 0);
    
    // argv[0] 不应该为空
    EXPECT_NE(args.argv[0], nullptr);
    
    // 打印所有参数
    for (int i = 0; i < args.argc; ++i) {
        printf("argv[%d]: %s\n", i, args.argv[i] ? args.argv[i] : "null");
    }
}

// 测试命令行参数数量合理性
TEST(CRT, ArgsCount)
{
    args_t args = aArgsGet();
    
    // argc 应该至少为 1（程序名）
    EXPECT_GE(args.argc, 1);
    
    // argc 不应该过大（sanity check）
    EXPECT_LT(args.argc, 10000);
}


GTEST_MAIN()
