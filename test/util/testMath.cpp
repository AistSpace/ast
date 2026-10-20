///
/// @file      testMath.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-27
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

#include "ast/Math.hpp"
#include "ast/Test.h"
#include "ast/Literals.hpp"


AST_USING_NAMESPACE
using namespace _AST literals;


TEST(MathTest, MathDegree)
{
    EXPECT_DOUBLE_EQ(sind(0.0), 0.0);
    EXPECT_DOUBLE_EQ(sind(1.0), sin(1.0_deg));

    EXPECT_DOUBLE_EQ(cosd(0.0), 1.0);
    EXPECT_DOUBLE_EQ(cosd(21.0), cos(21.0_deg));

    EXPECT_DOUBLE_EQ(tand(0.0), 0.0);
    EXPECT_DOUBLE_EQ(tand(41.0), tan(41.0_deg));
    
    EXPECT_DOUBLE_EQ(asind(0.0), 0.0);
    EXPECT_DOUBLE_EQ(asind(0.2), asin(0.2)/1_deg);

    EXPECT_DOUBLE_EQ(acosd(0.0), 90);
    EXPECT_DOUBLE_EQ(acosd(0.2), acos(0.2)/1_deg);

    EXPECT_DOUBLE_EQ(atand(100), atan(100)/1_deg);

    EXPECT_DOUBLE_EQ(atan2d(0.0, 0.0), 0.0);
    EXPECT_DOUBLE_EQ(atan2d(1.0, 1.0), 45.0);
    EXPECT_DOUBLE_EQ(atan2d(-1.0, 1.0), -45.0);
    EXPECT_DOUBLE_EQ(atan2d(-1.0, -1.0), -135.0);
    EXPECT_DOUBLE_EQ(atan2d(1.0, -1.0), 135.0);
    EXPECT_DOUBLE_EQ(atan2d(0.0, -1.0), 180.0);
    EXPECT_DOUBLE_EQ(atan2d(0.0, 1.0), 0.0);

}


TEST(MathTest, Round)
{
    EXPECT_DOUBLE_EQ(round(1.23456, 3), 1.235);
    EXPECT_DOUBLE_EQ(round(-1.23456, 3), -1.235);

    // 缩放因子溢出/下溢或缩放后上溢时，不应返回 inf 或 NaN
    EXPECT_DOUBLE_EQ(round(1.5, 400), 1.5);
    EXPECT_DOUBLE_EQ(round(1.5, -400), 0.0);
    EXPECT_DOUBLE_EQ(round(1e300, 10), 1e300);
    EXPECT_DOUBLE_EQ(round(1e300, -300), 1e300);
    EXPECT_DOUBLE_EQ(round(5.6e-309, 309), 6.0e-309);
    EXPECT_DOUBLE_EQ(round(std::numeric_limits<double>::infinity(), 3),
                     std::numeric_limits<double>::infinity());
    EXPECT_TRUE(std::isnan(round(std::numeric_limits<double>::quiet_NaN(), 3)));
}


GTEST_MAIN()


