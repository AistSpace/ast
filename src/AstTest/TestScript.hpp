///
/// @file      TestScript.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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
#include "AstUtil/Quantity.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class Quantity;

/// @brief 测试字符串表达式的解析
/// @details 测试字符串表达式的解析，包括输入表达式的解析和格式化表达式的解析
/// @param script 表达式字符串
/// @return errc_t 错误码
/// @note 该函数用于测试字符串表达式的解析
AST_TEST_CAPI errc_t aTestScriptParse(StringView script);

/// @brief 测试字符串表达式的语法错误
/// @param str 表达式字符串
/// @return errc_t 错误码
AST_TEST_CAPI errc_t aTestScriptSyntaxError(StringView str);

/// @brief 测试字符串表达式的运行时错误
/// @param str 表达式字符串
/// @return errc_t 错误码
AST_TEST_CAPI errc_t aTestScriptEvalRuntimeError(StringView str);

/// @brief 计算字符串表达式，期望结果为布尔值
/// @param str 表达式字符串
/// @param expectValue 期望的布尔值
/// @return errc_t 错误码
/// @note 该函数用于测试字符串表达式的解析和求值
AST_TEST_CAPI errc_t aTestScriptEvalBool(StringView str, bool expectValue);

/// @brief 计算字符串表达式，期望结果为整数
/// @param str 表达式字符串
/// @param expectedValue 期望的整数值
/// @return errc_t 错误码
/// @note 该函数用于测试字符串表达式的解析和求值
AST_TEST_CAPI errc_t aTestScriptEvalInt(StringView str, int expectedValue);


/// @brief 计算字符串表达式，期望结果为浮点数
/// @param str 表达式字符串
/// @param expectedValue 期望的浮点数值
/// @return errc_t 错误码
/// @note 该函数用于测试字符串表达式的解析和求值
AST_TEST_CAPI errc_t aTestScriptEvalDouble(StringView str, double expectedValue);

/// @brief 计算字符串表达式，期望结果为字符串
/// @param str 表达式字符串
/// @param expectedValue 期望的字符串值
/// @return errc_t 错误码
/// @note 该函数用于测试字符串表达式的解析和求值
AST_TEST_CAPI errc_t aTestScriptEvalString(StringView str, StringView expectedValue);

/// @brief 计算字符串表达式，期望结果为数量
/// @param str 表达式字符串
/// @param expectedValue 期望的数量值
/// @return errc_t 错误码
/// @note 该函数用于测试字符串表达式的解析和求值
AST_TEST_CAPI errc_t aTestScriptEvalQuantity(StringView str, const Quantity& expectedValue);


template<typename T>
errc_t aTestScriptEval(StringView str, T expectedValue);


template<>
errc_t aTestScriptEval<bool>(StringView str, bool expectedValue)
{
    return aTestScriptEvalBool(str, expectedValue);
}

template<>
errc_t aTestScriptEval<int>(StringView str, int expectedValue)
{
    return aTestScriptEvalInt(str, expectedValue);
}

template<>
errc_t aTestScriptEval<double>(StringView str, double expectedValue)
{
    return aTestScriptEvalDouble(str, expectedValue);
}

template<>
errc_t aTestScriptEval<StringView>(StringView str, StringView expectedValue)
{
    return aTestScriptEvalString(str, expectedValue);
}

template<>
errc_t aTestScriptEval<Quantity>(StringView str, Quantity expectedValue)
{
    return aTestScriptEvalQuantity(str, expectedValue);
}



/*! @} */

AST_NAMESPACE_END