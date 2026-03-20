///
/// @file      MockCache.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-24
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
/// 使用本软件所产生的风险，需由您自行承担。

#include "MockCache.hpp"

AST_NAMESPACE_BEGIN



int aMockCacheStatic(int k)
{
    static int int_key;
    static int int_value;
    if(int_key == k)
    {
        return int_value;
    }else{
        int_value = k;
        int_key = k;
        return int_value;
    }
}

int int_key;
int int_value;

int aMockCacheExtern(int k)
{
    if(int_key == k)
    {
        return int_value;
    }else{
        int_value = k;
        int_key = k;
        return int_value;
    }
}

int aMockCacheStaticTL(int k)
{
    static A_THREAD_LOCAL int int_key;
    static A_THREAD_LOCAL int int_value;
    if(int_key == k)
    {
        return int_value;
    }else{
        int_value = k;
        int_key = k;
        return int_value;
    }
}

A_THREAD_LOCAL int key_tl;
A_THREAD_LOCAL int value_tl;

int aMockCacheExternTL(int k)
{
    if(key_tl == k)
    {
        return value_tl;
    }else{
        value_tl = k;
        key_tl = k;
        return value_tl;
    }
}


AST_NAMESPACE_END
