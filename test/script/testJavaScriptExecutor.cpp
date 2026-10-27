///
/// @file      testJavaScriptExecutor.cpp
/// @brief     JavaScriptExecutor 测试
/// @details   测试脚本执行、表达式求值、变量传递、错误处理与实例隔离
/// @author    axel
/// @date      2026-10-27
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

#include "ast/ScriptExecutor.hpp"
#include "ast/ScriptExecutorProvider.hpp"
#include "ast/PluginRegistry.hpp"
#include "ast/Value.hpp"
#include "ast/Test.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <vector>

AST_USING_NAMESPACE

namespace {

/// @brief 扫描一次插件目录，再创建 JavaScript 执行器；插件缺失时返回空指针
/// @details 用显式扫描而不是依赖 aInitialize()，脚本执行器本身不需要其它初始化
std::unique_ptr<ScriptExecutor> newJavaScriptExecutor()
{
    static const errc_t scanResult = aScanPlugins();
    (void)scanResult;
    // 先查询接口，避免插件缺失时工厂逐次打印错误日志
    if(aPluginRegistry().find<IScriptExecutorProvider>("JavaScript") == nullptr)
        return nullptr;
    ScriptExecutor* scriptExecutor = aNewScriptExecutor(EScriptLanguage::eJavaScript);
    #if defined(AST_WITH_DUKTAPE)
    EXPECT_NE(scriptExecutor, nullptr);
    #endif
    return std::unique_ptr<ScriptExecutor>(scriptExecutor);
}

/// @brief 创建并初始化 JavaScript 执行器；插件或引擎不可用时返回空指针
std::unique_ptr<ScriptExecutor> newInitializedJavaScriptExecutor()
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
        return nullptr;
    if(executor->initialize() != eNoError)
        return nullptr;
    return executor;
}

} // namespace

TEST(JavaScriptExecutorTest, PluginProvidesInterface)
{
    static const errc_t scanResult = aScanPlugins();
    (void)scanResult;

    const std::vector<std::string> keys = aPluginRegistry().keys<IScriptExecutorProvider>();
    if(std::find(keys.begin(), keys.end(), std::string("JavaScript")) == keys.end())
    {
        GTEST_SKIP() << "AstPluginJavaScript is not available";
        return;
    }

    // 索引里有键却转换失败，说明接口 RTTI 没有跨 DLL 统一，是错误而非插件缺失
    IScriptExecutorProvider* provider = aPluginRegistry().find<IScriptExecutorProvider>("JavaScript");
    ASSERT_TRUE(provider != nullptr);

    std::unique_ptr<ScriptExecutor> executor(provider->create());
    ASSERT_TRUE(executor != nullptr);
    EXPECT_EQ(executor->initialize(), eNoError);
}


TEST(JavaScriptExecutorTest, VariableRoundTrip)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    if(executor->initialize() != eNoError)
        GTEST_SKIP() << "JavaScript engine initialization failed";

    EXPECT_EQ(executor->setVariable("name", "ast"), eNoError);
    EXPECT_EQ(executor->setVariable("value", 42.5), eNoError);
    EXPECT_EQ(executor->setVariable("count", 7), eNoError);
    EXPECT_EQ(executor->setVariable("flag", true), eNoError);

    std::string name;
    double value = 0.0;
    int count = 0;
    bool flag = false;
    EXPECT_EQ(executor->getVariable("name", name), eNoError);
    EXPECT_EQ(name, "ast");
    EXPECT_EQ(executor->getVariable("value", value), eNoError);
    EXPECT_DOUBLE_EQ(value, 42.5);
    EXPECT_EQ(executor->getVariable("count", count), eNoError);
    EXPECT_EQ(count, 7);
    EXPECT_EQ(executor->getVariable("flag", flag), eNoError);
    EXPECT_TRUE(flag);

    // JavaScript 只有一种数值类型，整数变量也能按 double 读回
    double countAsDouble = 0.0;
    EXPECT_EQ(executor->getVariable("count", countAsDouble), eNoError);
    EXPECT_DOUBLE_EQ(countAsDouble, 7.0);
}


TEST(JavaScriptExecutorTest, RepeatedAssignment)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    if(executor->initialize() != eNoError)
        GTEST_SKIP() << "JavaScript engine initialization failed";

    EXPECT_EQ(executor->setVariable("x", 1.5), eNoError);
    double number = 0.0;
    EXPECT_EQ(executor->getVariable("x", number), eNoError);
    EXPECT_DOUBLE_EQ(number, 1.5);

    // 同一变量可以切换类型
    EXPECT_EQ(executor->setVariable("x", "text"), eNoError);
    std::string text;
    EXPECT_EQ(executor->getVariable("x", text), eNoError);
    EXPECT_EQ(text, "text");

    EXPECT_EQ(executor->setVariable("x", false), eNoError);
    bool flag = true;
    EXPECT_EQ(executor->getVariable("x", flag), eNoError);
    EXPECT_FALSE(flag);
}


TEST(JavaScriptExecutorTest, ScriptVariableIsGlobal)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    if(executor->initialize() != eNoError)
        GTEST_SKIP() << "JavaScript engine initialization failed";

    // 脚本顶层的 var 声明应当成为全局变量（与 JScript 语义一致）
    EXPECT_EQ(executor->execute("var temp = 3; var doubled = temp * 2;"), eNoError);

    double doubled = 0.0;
    EXPECT_EQ(executor->getVariable("doubled", doubled), eNoError);
    EXPECT_DOUBLE_EQ(doubled, 6.0);
}


TEST(JavaScriptExecutorTest, HohmannScript)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    if(executor->initialize() != eNoError)
        GTEST_SKIP() << "JavaScript engine initialization failed";

    EXPECT_EQ(executor->setVariable("FinalRMag", 7000), eNoError);
    EXPECT_EQ(executor->setVariable("CurrentApoapsisMag", 6800), eNoError);
    EXPECT_EQ(executor->setVariable("CurrentEccentricity", 0.2), eNoError);
    EXPECT_EQ(executor->setVariable("CurrentTrueAnomaly", 2.1), eNoError);
    EXPECT_EQ(executor->setVariable("StopOnPeriapsisCoast", false), eNoError);
    EXPECT_EQ(executor->setVariable("StopOnApoapsisCoast", false), eNoError);
    EXPECT_EQ(executor->setVariable("StopOnPeriapsisTransfer", false), eNoError);
    EXPECT_EQ(executor->setVariable("StopOnApoapsisTransfer", false), eNoError);
    EXPECT_EQ(executor->setVariable("SkipInitialCoast", ""), eNoError);
    EXPECT_EQ(executor->setVariable("DesiredRMagScript", 0), eNoError);
    EXPECT_EQ(executor->setVariable("DesiredRMagDC", 0), eNoError);

    // 与 STK 霍曼转移场景中的 JScript 脚本一致
    const char* script = R"script(
        var startTrueAnomA;
        var startTrueAnomB;

        if (FinalRMag > CurrentApoapsisMag)
        {
            StopOnPeriapsisCoast = true;
            StopOnApoapsisCoast = false;
            StopOnPeriapsisTransfer = false;
            StopOnApoapsisTransfer = true;
            startTrueAnomA = 0.0;
            startTrueAnomB = 360.0;
        }
        else
        {
            StopOnPeriapsisCoast = false;
            StopOnApoapsisCoast = true;
            StopOnPeriapsisTransfer = true;
            StopOnApoapsisTransfer = false;
            startTrueAnomA = 180.0;
            startTrueAnomB = 180.0;
        }

        if ( CurrentEccentricity < 0.01 ||
             Math.abs(startTrueAnomA - CurrentTrueAnomaly) < 0.01 ||
             Math.abs(startTrueAnomB - CurrentTrueAnomaly) < 0.01 )
        {
            SkipInitialCoast = "Enable";
        }
        else
        {
            SkipInitialCoast = "Disable";
        }

        DesiredRMagScript = FinalRMag;
        DesiredRMagDC = FinalRMag;
    )script";
    EXPECT_EQ(executor->execute(script), eNoError);

    std::string skipInitialCoast;
    EXPECT_EQ(executor->getVariable("SkipInitialCoast", skipInitialCoast), eNoError);
    EXPECT_EQ(skipInitialCoast, "Disable");

    bool stopOnPeriapsisCoast = false;
    EXPECT_EQ(executor->getVariable("StopOnPeriapsisCoast", stopOnPeriapsisCoast), eNoError);
    EXPECT_TRUE(stopOnPeriapsisCoast);

    double desiredRMagScript = 0.0;
    EXPECT_EQ(executor->getVariable("DesiredRMagScript", desiredRMagScript), eNoError);
    EXPECT_DOUBLE_EQ(desiredRMagScript, 7000.0);

    double desiredRMagDC = 0.0;
    EXPECT_EQ(executor->getVariable("DesiredRMagDC", desiredRMagDC), eNoError);
    EXPECT_DOUBLE_EQ(desiredRMagDC, 7000.0);
}


TEST(JavaScriptExecutorTest, Evaluate)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    if(executor->initialize() != eNoError)
        GTEST_SKIP() << "JavaScript engine initialization failed";

    {
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("Math.sqrt(16)", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isDouble());
        EXPECT_DOUBLE_EQ(result.value_->toDouble(), 4.0);
    }
    {
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("'abc'", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isString());
        EXPECT_EQ(result.value_->toString(), "abc");
    }
    {
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("true", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isBool());
        EXPECT_TRUE(result.value_->toBool());
    }
    {
        // JavaScript 没有独立的整数类型
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("10 * 10", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isDouble());
        EXPECT_DOUBLE_EQ(result.value_->toDouble(), 100.0);
    }
}


TEST(JavaScriptExecutorTest, ErrorHandlingAndRecovery)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    if(executor->initialize() != eNoError)
        GTEST_SKIP() << "JavaScript engine initialization failed";

    // 运行时错误
    std::string error;
    EXPECT_NE(executor->execute("var a = notDefined + 1;", &error), eNoError);
    EXPECT_FALSE(error.empty());

    // 发生错误后仍然可以继续执行
    error.clear();
    EXPECT_EQ(executor->execute("var recovered = 5;", &error), eNoError);
    EXPECT_TRUE(error.empty());

    // 语法错误
    error.clear();
    EXPECT_NE(executor->execute("var broken = ;", &error), eNoError);
    EXPECT_FALSE(error.empty());

    error.clear();
    EXPECT_EQ(executor->execute("var ok = 1;", &error), eNoError);
    EXPECT_TRUE(error.empty());

    double recovered = 0.0;
    EXPECT_EQ(executor->getVariable("recovered", recovered), eNoError);
    EXPECT_DOUBLE_EQ(recovered, 5.0);
}


TEST(JavaScriptExecutorTest, NaNInfinity)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    if(executor->initialize() != eNoError)
        GTEST_SKIP() << "JavaScript engine initialization failed";

    EXPECT_EQ(executor->setVariable("nanValue", std::numeric_limits<double>::quiet_NaN()), eNoError);
    EXPECT_EQ(executor->setVariable("infValue", std::numeric_limits<double>::infinity()), eNoError);

    double nanValue = 0.0;
    EXPECT_EQ(executor->getVariable("nanValue", nanValue), eNoError);
    EXPECT_TRUE(std::isnan(nanValue));

    double infValue = 0.0;
    EXPECT_EQ(executor->getVariable("infValue", infValue), eNoError);
    EXPECT_TRUE(std::isinf(infValue));
}


TEST(JavaScriptExecutorTest, IntConversions)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    if(executor->initialize() != eNoError)
        GTEST_SKIP() << "JavaScript engine initialization failed";

    // 小数按向零截断转换
    EXPECT_EQ(executor->setVariable("fraction", 3.9), eNoError);
    int integer = 0;
    EXPECT_EQ(executor->getVariable("fraction", integer), eNoError);
    EXPECT_EQ(integer, 3);

    // 非有限值与超出 int 范围的值应报错
    EXPECT_EQ(executor->setVariable("huge", 1e20), eNoError);
    EXPECT_EQ(executor->getVariable("huge", integer), eErrorOutOfRange);

    EXPECT_EQ(executor->setVariable("nanInt", std::numeric_limits<double>::quiet_NaN()), eNoError);
    EXPECT_EQ(executor->getVariable("nanInt", integer), eErrorOutOfRange);
}


TEST(JavaScriptExecutorTest, StrictTypeConversions)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    if(executor->initialize() != eNoError)
        GTEST_SKIP() << "JavaScript engine initialization failed";

    // 字符串不能按数值读取，字符串读取会走基类的回退路径
    EXPECT_EQ(executor->setVariable("text", "enable"), eNoError);
    double number = -1.0;
    EXPECT_EQ(executor->getVariable("text", number), eErrorInvalidType);
    bool flag = true;
    EXPECT_EQ(executor->getVariable("text", flag), eErrorInvalidType);

    // 布尔值可以按 0/1 读取
    EXPECT_EQ(executor->setVariable("enabled", true), eNoError);
    EXPECT_EQ(executor->getVariable("enabled", number), eNoError);
    EXPECT_DOUBLE_EQ(number, 1.0);

    // 不存在的变量
    EXPECT_EQ(executor->getVariable("missing", number), eErrorNotFound);
}


TEST(JavaScriptExecutorTest, IsolatedInstances)
{
    std::unique_ptr<ScriptExecutor> first = newJavaScriptExecutor();
    std::unique_ptr<ScriptExecutor> second = newJavaScriptExecutor();
    if(!first || !second)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    if(first->initialize() != eNoError || second->initialize() != eNoError)
        GTEST_SKIP() << "JavaScript engine initialization failed";

    EXPECT_EQ(first->setVariable("shared", 1.0), eNoError);
    EXPECT_EQ(second->setVariable("shared", 2.0), eNoError);
    EXPECT_EQ(first->execute("var onlyFirst = 10;"), eNoError);

    double value = 0.0;
    EXPECT_EQ(first->getVariable("shared", value), eNoError);
    EXPECT_DOUBLE_EQ(value, 1.0);
    EXPECT_EQ(second->getVariable("shared", value), eNoError);
    EXPECT_DOUBLE_EQ(value, 2.0);
    EXPECT_EQ(first->getVariable("onlyFirst", value), eNoError);
    EXPECT_DOUBLE_EQ(value, 10.0);
    EXPECT_EQ(second->getVariable("onlyFirst", value), eErrorNotFound);
}


TEST(JavaScriptExecutorTest, ExecuteBeforeInitialize)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }

    EXPECT_EQ(executor->execute("var x = 1;"), eErrorNotInit);
    EXPECT_EQ(executor->evaluate("1 + 1"), eErrorNotInit);
    EXPECT_EQ(executor->setVariable("x", 1.0), eErrorNotInit);
    double value = 0.0;
    EXPECT_EQ(executor->getVariable("x", value), eErrorNotInit);
}


TEST(JavaScriptExecutorTest, InitializeTwiceAndFinalize)
{
    std::unique_ptr<ScriptExecutor> executor = newJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }

    // 重复初始化不应重建堆，也不应丢失已有的全局变量
    EXPECT_EQ(executor->initialize(), eNoError);
    EXPECT_EQ(executor->execute("var beforeReinit = 1;"), eNoError);
    EXPECT_EQ(executor->initialize(), eNoError);

    double beforeReinit = 0.0;
    EXPECT_EQ(executor->getVariable("beforeReinit", beforeReinit), eNoError);
    EXPECT_DOUBLE_EQ(beforeReinit, 1.0);

    // 先制造一次错误，确认 finalize 会清空最近一次的错误信息
    EXPECT_NE(executor->execute("var broken = ;"), eNoError);
    EXPECT_FALSE(executor->getLastError().empty());

    // finalize 之后所有接口都应报告未初始化
    executor->finalize();
    EXPECT_TRUE(executor->getLastError().empty());

    EXPECT_EQ(executor->execute("var x = 1;"), eErrorNotInit);
    EXPECT_EQ(executor->evaluate("1 + 1"), eErrorNotInit);
    EXPECT_EQ(executor->setVariable("x", "text"), eErrorNotInit);
    EXPECT_EQ(executor->setVariable("x", 1.0), eErrorNotInit);
    EXPECT_EQ(executor->setVariable("x", 1), eErrorNotInit);
    EXPECT_EQ(executor->setVariable("x", true), eErrorNotInit);
    EXPECT_FALSE(executor->getLastError().empty());

    std::string text;
    double number = 0.0;
    int integer = 0;
    bool flag = false;
    EXPECT_EQ(executor->getVariable("x", text), eErrorNotInit);
    EXPECT_EQ(executor->getVariable("x", number), eErrorNotInit);
    EXPECT_EQ(executor->getVariable("x", integer), eErrorNotInit);
    EXPECT_EQ(executor->getVariable("x", flag), eErrorNotInit);

    // finalize 后可以重新初始化并继续使用
    EXPECT_EQ(executor->initialize(), eNoError);
    EXPECT_EQ(executor->execute("var afterReinit = 2;"), eNoError);
    double afterReinit = 0.0;
    EXPECT_EQ(executor->getVariable("afterReinit", afterReinit), eNoError);
    EXPECT_DOUBLE_EQ(afterReinit, 2.0);
}


TEST(JavaScriptExecutorTest, EmptyVariableName)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }

    // 空变量名对每种重载都应被拒绝，而不是退化成给全局对象的空键赋值
    EXPECT_EQ(executor->setVariable("", "value"), eErrorInvalidParam);
    EXPECT_EQ(executor->setVariable("", 1.5), eErrorInvalidParam);
    EXPECT_EQ(executor->setVariable("", 2), eErrorInvalidParam);
    EXPECT_EQ(executor->setVariable("", true), eErrorInvalidParam);
    EXPECT_FALSE(executor->getLastError().empty());
}


TEST(JavaScriptExecutorTest, StringVariableConversions)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }

    // 数值不能按字符串读取
    EXPECT_EQ(executor->setVariable("number", 12.5), eNoError);
    std::string text = "unchanged";
    EXPECT_EQ(executor->getVariable("number", text), eErrorInvalidType);
    EXPECT_EQ(text, "unchanged");

    // 布尔按 "true"/"false" 读取
    EXPECT_EQ(executor->setVariable("enabled", true), eNoError);
    EXPECT_EQ(executor->getVariable("enabled", text), eNoError);
    EXPECT_EQ(text, "true");

    EXPECT_EQ(executor->setVariable("disabled", false), eNoError);
    EXPECT_EQ(executor->getVariable("disabled", text), eNoError);
    EXPECT_EQ(text, "false");

    // 不存在的变量
    EXPECT_EQ(executor->getVariable("missing", text), eErrorNotFound);
}


TEST(JavaScriptExecutorTest, IntegerAndBoolConversions)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }

    // 布尔可以按 1/0 读取为整数
    EXPECT_EQ(executor->setVariable("yes", true), eNoError);
    int integer = -1;
    EXPECT_EQ(executor->getVariable("yes", integer), eNoError);
    EXPECT_EQ(integer, 1);

    EXPECT_EQ(executor->setVariable("no", false), eNoError);
    EXPECT_EQ(executor->getVariable("no", integer), eNoError);
    EXPECT_EQ(integer, 0);

    // 字符串不能按整数读取
    EXPECT_EQ(executor->setVariable("text", "abc"), eNoError);
    EXPECT_EQ(executor->getVariable("text", integer), eErrorInvalidType);
    EXPECT_EQ(executor->getVariable("missingInt", integer), eErrorNotFound);

    // 数值按 JavaScript 的真值语义转为布尔：0 与 NaN 为假
    bool flag = true;
    EXPECT_EQ(executor->setVariable("zero", 0.0), eNoError);
    EXPECT_EQ(executor->getVariable("zero", flag), eNoError);
    EXPECT_FALSE(flag);

    EXPECT_EQ(executor->setVariable("nonzero", 2.5), eNoError);
    EXPECT_EQ(executor->getVariable("nonzero", flag), eNoError);
    EXPECT_TRUE(flag);

    EXPECT_EQ(executor->setVariable("nanBool", std::numeric_limits<double>::quiet_NaN()), eNoError);
    EXPECT_EQ(executor->getVariable("nanBool", flag), eNoError);
    EXPECT_FALSE(flag);

    // 字符串不能按布尔读取
    EXPECT_EQ(executor->getVariable("text", flag), eErrorInvalidType);
    EXPECT_EQ(executor->getVariable("missingBool", flag), eErrorNotFound);
}


TEST(JavaScriptExecutorTest, EvaluateNonPrimitiveValues)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }

    {
        // null 与 undefined 都是空值
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("null", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isNull());
    }
    {
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("undefined", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isNull());
    }
    {
        // 对象与数组走字符串回退路径
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("({answer: 42})", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isString());
        EXPECT_EQ(result.value_->toString(), "[object Object]");
    }
    {
        ScriptResult result;
        EXPECT_EQ(executor->evaluate("[1, 2, 3]", &result), eNoError);
        ASSERT_TRUE(result.value_ != nullptr);
        EXPECT_TRUE(result.value_->isString());
        EXPECT_EQ(result.value_->toString(), "1,2,3");
    }
}


TEST(JavaScriptExecutorTest, ErrorValuesWithoutStack)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }

    // 抛出非对象值：错误信息就是该值本身，没有 stack 可附带
    std::string error;
    EXPECT_NE(executor->execute("throw 'plain string error';", &error), eNoError);
    EXPECT_EQ(error, "plain string error");
    std::printf("error: %s\n", error.c_str());

    // Error 对象附带调用栈，错误信息应当是「消息 + 换行 + stack」
    error.clear();
    EXPECT_NE(executor->execute("throw new Error('with stack');", &error), eNoError);
    EXPECT_NE(error.find('\n'), std::string::npos);
    std::printf("error: %s\n", error.c_str());

    // 抛出没有 stack 属性的普通对象
    error.clear();
    EXPECT_NE(executor->execute("throw {message: 'custom'};", &error), eNoError);
    EXPECT_FALSE(error.empty());
    EXPECT_EQ(error.find('\n'), std::string::npos);
    std::printf("error: %s\n", error.c_str());

    // stack 存在但不是字符串，只保留错误信息本身
    error.clear();
    EXPECT_NE(executor->execute("var err = new Error('with stack'); err.stack = 42; throw err;", &error), eNoError);
    EXPECT_FALSE(error.empty());
    EXPECT_EQ(error.find('\n'), std::string::npos);
    std::printf("error: %s\n", error.c_str());

    // evaluate 的错误同样写入 ScriptResult
    ScriptResult result;
    EXPECT_NE(executor->evaluate("throw new Error('eval failed')", &result), eNoError);
    EXPECT_FALSE(result.error_.empty());
    std::printf("error: %s\n", result.error_.c_str());

    // 出错后引擎状态仍然可用
    EXPECT_EQ(executor->execute("var recoveredAfterThrow = 9;"), eNoError);
    double recovered = 0.0;
    EXPECT_EQ(executor->getVariable("recoveredAfterThrow", recovered), eNoError);
    EXPECT_DOUBLE_EQ(recovered, 9.0);
}


TEST(JavaScriptExecutorTest, EmptyScriptAndExpression)
{
    std::unique_ptr<ScriptExecutor> executor = newInitializedJavaScriptExecutor();
    if(!executor)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }

    // 默认构造的 StringView 数据指针为空，覆盖空脚本/空表达式的处理
    EXPECT_EQ(executor->execute(StringView()), eNoError);
    EXPECT_EQ(executor->execute(StringView("")), eNoError);

    ScriptResult result;
    EXPECT_EQ(executor->evaluate(StringView(), &result), eNoError);
    // 空表达式的值等价于 undefined
    ASSERT_TRUE(result.value_ != nullptr);
    EXPECT_TRUE(result.value_->isNull());

    // 空值可以作为字符串变量写入
    EXPECT_EQ(executor->setVariable("empty", StringView()), eNoError);
    std::string empty = "not empty";
    EXPECT_EQ(executor->getVariable("empty", empty), eNoError);
    EXPECT_TRUE(empty.empty());
}


TEST(JavaScriptExecutorTest, FactoryFallbacks)
{
    // 不存在的插件名返回空指针并输出错误日志
    EXPECT_EQ(aNewScriptExecutorFromPlugin("NoSuchScriptExecutor"), nullptr);

#if !defined(_WIN32)
    // 非 Windows 平台没有 Active Scripting，eJScript 由可移植的 JavaScript 引擎执行
    std::unique_ptr<ScriptExecutor> probe = newJavaScriptExecutor();
    if(!probe)
    {
        GTEST_SKIP() << "JavaScript engine is not available";
        return;
    }
    std::unique_ptr<ScriptExecutor> jscript(aNewScriptExecutor(EScriptLanguage::eJScript));
    ASSERT_TRUE(jscript != nullptr);
    EXPECT_EQ(jscript->initialize(), eNoError);
    EXPECT_EQ(jscript->execute("var routedToPlugin = 4;"), eNoError);
    double routedToPlugin = 0.0;
    EXPECT_EQ(jscript->getVariable("routedToPlugin", routedToPlugin), eNoError);
    EXPECT_DOUBLE_EQ(routedToPlugin, 4.0);
#endif
}


GTEST_MAIN()
