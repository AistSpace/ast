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
    return std::unique_ptr<ScriptExecutor>(aNewScriptExecutor(EScriptLanguage::eJavaScript));
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


GTEST_MAIN()
