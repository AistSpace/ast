///
/// @file      TestScript.cpp
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

#include "TestScript.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstScript/ValString.hpp"
#include "AstScript/ValInt.hpp"
#include "AstScript/Interpreter.hpp"


AST_NAMESPACE_BEGIN

errc_t aTestScriptParse(StringView script)
{
    Expr* expr1 = aParseExpr(script);
    if(!expr1)
    {
        aError("failed to parse input expr: %s", script.data());
        return eErrorInvalidParam;
    }
    std::string exprStr1 = aFormatExpr(expr1);
    Expr* expr2 = aParseExpr(exprStr1);
    if(!expr2)
    {
        aError("failed to parse formatted expr: %s", exprStr1.c_str());
        return eErrorInvalidParam;
    }
    std::string exprStr2 = aFormatExpr(expr2);
    if(exprStr1 != exprStr2)
    {
        aError("1st formatted expr not equal to 2nd formatted expr");
        return eErrorInvalidParam;
    }
    return eNoError;
}

errc_t aTestScriptSyntaxError(StringView str)
{
    Expr* expr = aParseExpr(str);
    if(expr)
    {
        aError("expect syntax error");
        return eErrorInvalidParam;
    }
    return eNoError;
}

errc_t aTestScriptEvalRuntimeError(StringView str)
{
    Expr* expr = aParseExpr(str);
    if(!expr)
    {
        aError("unexpected error when parse expr: '%.*s'", str.size(), str.data());
        return eErrorInvalidParam;
    }
    SharedPtr<Value> value = aEvalExpr(expr);
    if(value)
    {
        aError("expect runtime error");
        return eErrorInvalidParam;
    }
    return eNoError;
}

errc_t aTestScriptEvalBool(StringView str, bool expectValue)
{
    SharedPtr<Value> value = aEval(str);
    if(!value)
    {
        aError("failed to eval bool: '%.*s'", str.size(), str.data());
        return eErrorNullPtr;
    }
    if(value.get()){
        if(!aValueIsBool(value.get()))
        {
            aError("expect bool value");
            return eErrorInvalidType;
        }
        bool actualValue = aValueUnboxBool(value.get());
        if(actualValue != expectValue)
        {
            aError("expect bool value %d, but get %d", expectValue, actualValue);
            return eErrorInvalidValue;
        }
    }
    return eNoError;
}

errc_t aTestScriptEvalInt(StringView str, int expectedValue)
{
    SharedPtr<Value> value = aEval(str);
    if(!value)
    {
        aError("failed to eval int: '%.*s'", str.size(), str.data());
        return eErrorNullPtr;
    }
    if(value.get()){
        if(!aValueIsInt(value.get()))
        {
            aError("expect int value");
            return eErrorInvalidType;
        }
        int actualValue = aValueUnboxInt(value.get());
        if(actualValue != expectedValue)
        {
            aError("expect int value %d, but get %d", expectedValue, actualValue);
            return eErrorInvalidValue;
        }
    }
    return eNoError;
}


errc_t aTestScriptEvalDouble(StringView str, double expectedValue)
{
    SharedPtr<Value> value = aEval(str);
    if(!value)
    {
        aError("failed to eval double: '%.*s'", str.size(), str.data());
        return eErrorNullPtr;
    }
    if(value.get()){
        if(!aValueIsDouble(value.get()))
        {
            aError("expect double value");
            return eErrorInvalidType;
        }
        double actualValue = aValueUnboxDouble(value.get());
        if(actualValue != expectedValue)
        {
            aError("expect double value %f, but get %f", expectedValue, actualValue);
            return eErrorInvalidValue;
        }
    }
    return eNoError;
}

errc_t aTestScriptEvalString(StringView str, StringView expectedValue)
{
    SharedPtr<Value> value = aEval(str);
    if(!value)
    {
        aError("failed to eval string: %s", str.data());
        return eErrorNullPtr;
    }
    if(value.get()){
        if(auto valString = dynamic_cast<ValString*>(value.get()))
        {
            if(valString->value() != std::string(expectedValue))
            {
                aError("expect string value %s, but get %s", expectedValue.data(), valString->value().c_str());
                return eErrorInvalidValue;
            }
        }else{
            aError("expect string value");
            return eErrorInvalidType;
        }
    }
    return eNoError;
}

errc_t aTestScriptEvalQuantity(StringView str, const Quantity &expectedValue)
{
    SharedPtr<Value> value = aEval(str);
    if(!value)
    {
        aError("failed to eval quantity: '%.*s'", str.size(), str.data());
        return eErrorNullPtr;
    }
    if(value.get()){
        if(!aValueIsQuantity(value.get()))
        {
            aError("expect quantity value");
            return eErrorInvalidType;
        }
        Quantity actualValue = aValueUnboxQuantity(value.get());
        if(actualValue != expectedValue)
        {
            aError("expect quantity value %s, but get %s", expectedValue.toString().c_str(), actualValue.toString().c_str());
            return eErrorInvalidValue;
        }
    }
    return eNoError;
}

AST_NAMESPACE_END
