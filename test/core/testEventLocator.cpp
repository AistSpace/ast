///
/// @file      testEventLocator.cpp
/// @brief     事件区间定位测试
/// @details   测试 aFindEventIntervals：符号翻转检测、同号步内局部极值穿越、Brent/二分法精化、非负区间输出
/// @author    axel
/// @date      2026-10-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "ast/Constants.hpp"
#include "ast/EventLocator.hpp"
#include "ast/FixedStepStepper.hpp"
#include "ast/Interval.hpp"
#include "ast/IntervalList.hpp"
#include "ast/UnaryScalarFunc.hpp"
#include "ast/Test.hpp"
#include <cmath>
#include <functional>

AST_USING_NAMESPACE

namespace {

/// @brief 由可调用对象构造的一元标量函数
class MockScalarFunc : public UnaryScalarFunc
{
public:
    explicit MockScalarFunc(std::function<double(double)> func) : func_(std::move(func)) {}

    void evaluate(double x, double* out) const override { *out = func_(x); }

private:
    std::function<double(double)> func_;
};

} // anonymous namespace

// 全部为正：整个区间都应满足，输出单个区间
TEST(EventLocatorTest, AllPositive)
{
    MockScalarFunc func([](double) { return 1.0; });
    FixedStepStepper stepper(10.0);
    Interval interval{0.0, 100.0};
    IntervalList intervals;

    errc_t rc = aFindEventIntervals(func, stepper, interval, intervals);

    EXPECT_EQ(rc, eNoError);
    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_DOUBLE_EQ(intervals[0].stop(), 100.0);
}

// 全部为负：无事件区间
TEST(EventLocatorTest, AllNegative)
{
    MockScalarFunc func([](double) { return -1.0; });
    FixedStepStepper stepper(10.0);
    Interval interval{0.0, 100.0};
    IntervalList intervals;

    errc_t rc = aFindEventIntervals(func, stepper, interval, intervals);

    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(intervals.size(), 0u);
}

// 步长大于区间长度：采样点只有起点和终点两个
TEST(EventLocatorTest, SingleStepWholeInterval)
{
    MockScalarFunc func([](double x) { return x - 5.0; });
    FixedStepStepper stepper(20.0); // 步长 20 > 区间长度 10
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 5.0, 1e-6);
    EXPECT_DOUBLE_EQ(intervals[0].stop(), 10.0);
}

// 同上的单步情形，但整段区间都在事件区内
TEST(EventLocatorTest, SingleStepAllPositive)
{
    MockScalarFunc func([](double) { return 1.0; });
    FixedStepStepper stepper(20.0); // 步长 20 > 区间长度 10
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    errc_t rc = aFindEventIntervals(func, stepper, interval, intervals);

    EXPECT_EQ(rc, eNoError);
    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_DOUBLE_EQ(intervals[0].stop(), 10.0);
}

// 单步且整段区间都不在事件区内：无输出
TEST(EventLocatorTest, SingleStepAllOutside)
{
    MockScalarFunc func([](double) { return -1.0; });
    FixedStepStepper stepper(20.0); // 步长 20 > 区间长度 10
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    errc_t rc = aFindEventIntervals(func, stepper, interval, intervals);

    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(intervals.size(), 0u);
}

// 单步、两端采样都满足，但步内是凹陷：中间有一段需要排除的事件外区间
// f>=0 当且仅当 |x-5| >= 2，事件区应为 [0, 3] 与 [7, 10]
TEST(EventLocatorTest, SingleStepDipInside)
{
    MockScalarFunc func([](double x) {
        double d = x - 5.0;
        return 0.1 * d * d - 0.4; // f(0)=f(10)=2.1>0，凹陷谷值 -0.4 在 x=5
    });
    FixedStepStepper stepper(20.0); // 步长 20 > 区间长度 10
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 2u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_NEAR(intervals[0].stop(), 3.0, 1e-6);
    EXPECT_NEAR(intervals[1].start(), 7.0, 1e-6);
    EXPECT_DOUBLE_EQ(intervals[1].stop(), 10.0);
}

// 单步、两端采样都不满足，但步内是凸起：中间有一段事件区间
// f>=0 当且仅当 |x-5| <= 2，事件区应为 [3, 7]
TEST(EventLocatorTest, SingleStepBumpInside)
{
    MockScalarFunc func([](double x) {
        double d = x - 5.0;
        return 0.4 - 0.1 * d * d; // f(0)=f(10)=-2.1<0，凸起峰值 0.4 在 x=5
    });
    FixedStepStepper stepper(20.0); // 步长 20 > 区间长度 10
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 3.0, 1e-6);
    EXPECT_NEAR(intervals[0].stop(), 7.0, 1e-6);
}

// 线性函数：过零点恰落在采样网格上，走二分法分支
TEST(EventLocatorTest, LinearZeroOnGrid)
{
    MockScalarFunc func([](double x) { return x - 5.0; });
    FixedStepStepper stepper(1.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 5.0, 1e-6);
    EXPECT_DOUBLE_EQ(intervals[0].stop(), 10.0);
}

// sin(x) 在 [0, 2π]：非负段为 [0, π]，两端都非零，走 Brent 求根
TEST(EventLocatorTest, SineSingleInterval)
{
    MockScalarFunc func([](double x) { return std::sin(x); });
    FixedStepStepper stepper(0.1);
    Interval interval{0.0, 2.0 * kPI};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);
    printf("intervals: %s\n", intervals.toString().c_str());

    EXPECT_EQ(intervals.size(), 1u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_NEAR(intervals[0].stop(), kPI, 1e-6);
}

// cos(x) 在 [0, 2π]：非负段为 [0, π/2] 和 [3π/2, 2π]，共两个区间
TEST(EventLocatorTest, CosineTwoIntervals)
{
    MockScalarFunc func([](double x) { return std::cos(x); });
    FixedStepStepper stepper(0.1);
    Interval interval{0.0, 2.0 * kPI};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 2u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_NEAR(intervals[0].stop(), 0.5 * kPI, 1e-6);
    EXPECT_NEAR(intervals[1].start(), 1.5 * kPI, 1e-6);
    EXPECT_NEAR(intervals[1].stop(), 2.0 * kPI, 1e-6);
}

// 两个过零点都落在区间内部：负 → 非负 → 负，输出中间那一段
TEST(EventLocatorTest, ShiftedSine)
{
    MockScalarFunc func([](double x) { return std::sin(x - 1.0); });
    FixedStepStepper stepper(0.1);
    Interval interval{0.0, 5.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    EXPECT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 1.0, 1e-6);
    EXPECT_NEAR(intervals[0].stop(), 1.0 + kPI, 1e-6);
}

// 同号步内的局部极值：采样两端同号，但步内极值跨越零，需要额外检出/排除
// 以下四个用例的极值都完整落在单个步长 (5, 6) 或 (0, 1) 之间，采样点全部同号

// 由负到负：两端采样都为负，但步内存在正的凸起，中间其实有一段事件区间
TEST(EventLocatorTest, BumpInsideNegativeStep)
{
    // 凸起中心 5.5、半宽 0.3、峰值 0.4：f>=0 当且仅当 x ∈ 5.5 ± 0.3*sqrt(0.4)
    MockScalarFunc func([](double x) {
        double d = (x - 5.5) / 0.3;
        return 0.4 - d * d;
    });
    FixedStepStepper stepper(1.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    const double halfWidth = 0.3 * std::sqrt(0.4);
    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 5.5 - halfWidth, 1e-6);
    EXPECT_NEAR(intervals[0].stop(), 5.5 + halfWidth, 1e-6);
}

// 由正到正：两端采样都为正，但步内存在负的凹陷，中间有一段区间需要排除
TEST(EventLocatorTest, DipInsidePositiveStep)
{
    // 凹陷中心 5.5、半宽 0.3、谷值 -0.4：f<0 当且仅当 x ∈ 5.5 ± 0.3*sqrt(0.4)
    MockScalarFunc func([](double x) {
        double d = (x - 5.5) / 0.3;
        return d * d - 0.4;
    });
    FixedStepStepper stepper(1.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    printf("intervals: %s\n", intervals.toString().c_str());

    const double halfWidth = 0.3 * std::sqrt(0.4);
    ASSERT_EQ(intervals.size(), 2u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_NEAR(intervals[0].stop(), 5.5 - halfWidth, 1e-6);
    EXPECT_NEAR(intervals[1].start(), 5.5 + halfWidth, 1e-6);
    EXPECT_DOUBLE_EQ(intervals[1].stop(), 10.0);
}

// 步内极值恰好落在零上（相切、不跨越）：不应误判出区间，也不应把整段分裂
TEST(EventLocatorTest, ExtremumTouchingZeroNoSplit)
{
    MockScalarFunc func([](double x) {
        double d = (x - 5.5) / 0.3;
        return d * d; // 恒非负，仅 x=5.5 处恰好为 0
    });
    FixedStepStepper stepper(1.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_DOUBLE_EQ(intervals[0].stop(), 10.0);
}

// 同号步内的极值偏向步的一端（此处贴近步右端）时仍能正确检出
TEST(EventLocatorTest, BumpOffCenterInsideStep)
{
    // 凸起中心 5.85、半宽 0.1、峰值 0.3，完整落在步 (5, 6) 内
    MockScalarFunc func([](double x) {
        double d = (x - 5.85) / 0.1;
        return 0.3 - d * d;
    });
    FixedStepStepper stepper(1.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    const double halfWidth = 0.1 * std::sqrt(0.3);
    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 5.85 - halfWidth, 1e-6);
    EXPECT_NEAR(intervals[0].stop(), 5.85 + halfWidth, 1e-6);
}

// 凹陷藏在相邻采样点之间（采样点均落在凹陷外且同号），采样序列在凹陷两侧反向下弯
TEST(EventLocatorTest, DipBetweenSamples)
{
    // 凹陷中心 0.32、半宽 0.1、谷值 -0.25：f<0 当且仅当 x ∈ (0.27, 0.37)
    // 步长 0.2 的采样点为 0/0.2/0.4/0.6/0.8/1.0，均落在凹陷外
    MockScalarFunc func([](double x) {
        double d = (x - 0.32) / 0.1;
        return d * d - 0.25;
    });
    FixedStepStepper stepper(0.2);
    Interval interval{0.0, 1.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 2u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_NEAR(intervals[0].stop(), 0.27, 1e-6);
    EXPECT_NEAR(intervals[1].start(), 0.37, 1e-6);
    EXPECT_DOUBLE_EQ(intervals[1].stop(), 1.0);
}

// 事件区间内的极大值（凸向正侧）不应造成分裂
TEST(EventLocatorTest, PositiveBumpKeepsInterval)
{
    // 极大值 1.0 在 x=5.5，f>=0 当且仅当 x ∈ (5.5 ± 0.3*sqrt(10))
    MockScalarFunc func([](double x) {
        double d = (x - 5.5) / 0.3;
        return 1.0 - 0.1 * d * d;
    });
    FixedStepStepper stepper(1.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    const double halfWidth = 0.3 * std::sqrt(10.0);
    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 5.5 - halfWidth, 1e-6);
    EXPECT_NEAR(intervals[0].stop(), 5.5 + halfWidth, 1e-6);
}

// 非事件区间内的极大值（未触及零）不应产生区间
TEST(EventLocatorTest, NegativeBumpWithoutCrossing)
{
    // 极大值 -0.1 在 x=5.5，函数恒为负
    MockScalarFunc func([](double x) {
        double d = (x - 5.5) / 0.3;
        return -0.1 - d * d;
    });
    FixedStepStepper stepper(1.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    EXPECT_EQ(intervals.size(), 0u);
}

// 空区间（反向区间）直接返回空结果
TEST(EventLocatorTest, EmptyInterval)
{
    MockScalarFunc func([](double) { return 1.0; });
    FixedStepStepper stepper(1.0);
    Interval interval{10.0, 0.0};
    IntervalList intervals;

    errc_t rc = aFindEventIntervals(func, stepper, interval, intervals);

    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(intervals.size(), 0u);
}

// 事件在末段内结束，区间不能无条件延伸到区间终点
// 步长 3 的采样点为 0/3/6/9，区间终点 10 未被采样，穿越点 9.5 落在末段 (9, 10) 内
TEST(EventLocatorTest, EventEndsInsideTailStep)
{
    MockScalarFunc func([](double x) { return 9.5 - x; });
    FixedStepStepper stepper(3.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_NEAR(intervals[0].stop(), 9.5, 1e-6);
}

// 事件区间整体藏在末段内：采样点 9 仍在事件区内，但事件已在终点之前结束
// 事件区为 [8, 9.2]，末段 (9, 10) 内的穿越点为 9.2，应输出 [8, 9.2] 而非 [8, 10]
TEST(EventLocatorTest, BumpEndsInsideTailStep)
{
    MockScalarFunc func([](double x) {
        double d = x - 8.6;
        return -(d * d - 0.36);
    });
    FixedStepStepper stepper(3.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 8.0, 1e-6);
    EXPECT_NEAR(intervals[0].stop(), 9.2, 1e-6);
}

// 凸起整体藏在末段内：采样点 9 与区间终点 10 都为负，但两者之间有一段事件区间
// 终点补入循环后，末段 (9, 10) 也在极值检测的覆盖范围内
// 事件区为 [9.3 ± 0.3*sqrt(0.4)]，即约 [9.1103, 9.4897]
TEST(EventLocatorTest, BumpInsideTailStep)
{
    MockScalarFunc func([](double x) {
        double d = (x - 9.3) / 0.3;
        return 0.4 - d * d;
    });
    FixedStepStepper stepper(3.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    const double halfWidth = 0.3 * std::sqrt(0.4);
    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 9.3 - halfWidth, 1e-6);
    EXPECT_NEAR(intervals[0].stop(), 9.3 + halfWidth, 1e-6);
}

// 凹陷藏在末段内：采样点 9 与区间终点 10 都在事件区内，但两者之间有一段需要排除的事件外区间
// 事件区为 [0, 9.5 - 0.3*sqrt(0.4)] 与 [9.5 + 0.3*sqrt(0.4), 10]
TEST(EventLocatorTest, DipInsideTailStep)
{
    MockScalarFunc func([](double x) {
        double d = (x - 9.5) / 0.3;
        return d * d - 0.4;
    });
    FixedStepStepper stepper(3.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    const double halfWidth = 0.3 * std::sqrt(0.4);
    ASSERT_EQ(intervals.size(), 2u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_NEAR(intervals[0].stop(), 9.5 - halfWidth, 1e-6);
    EXPECT_NEAR(intervals[1].start(), 9.5 + halfWidth, 1e-6);
    EXPECT_DOUBLE_EQ(intervals[1].stop(), 10.0);
}

// 事件在首段内开始，区间不能无条件回退到区间起点
// 步长 3 的采样点为 0/3/6/9/10，穿越点 0.5 落在首段 (0, 3) 内
TEST(EventLocatorTest, EventStartsInsideHeadStep)
{
    MockScalarFunc func([](double x) { return x - 0.5; });
    FixedStepStepper stepper(3.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 0.5, 1e-6);
    EXPECT_DOUBLE_EQ(intervals[0].stop(), 10.0);
}

// 事件区间的起点藏在首段内：采样点 0 不在事件区内，而采样点 3 已进入事件区
// 事件区为 [2, 4]，首段 (0, 3) 内的穿越点为 2.0，应输出 [2, 4] 而非 [0, 4]
TEST(EventLocatorTest, BumpStartsInsideHeadStep)
{
    MockScalarFunc func([](double x) {
        double d = x - 3.0;
        return -(d * d - 1.0);
    });
    FixedStepStepper stepper(3.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 2.0, 1e-6);
    EXPECT_NEAR(intervals[0].stop(), 4.0, 1e-6);
}

// 凸起整体藏在首段内：采样点 0 与采样点 3 都为负，但两者之间有一段事件区间
TEST(EventLocatorTest, BumpInsideHeadStep)
{
    MockScalarFunc func([](double x) {
        double d = (x - 0.3) / 0.3;
        return 0.4 - d * d;
    });
    FixedStepStepper stepper(3.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);
    printf("intervals: %s\n", intervals.toString().c_str());

    const double halfWidth = 0.3 * std::sqrt(0.4);
    ASSERT_EQ(intervals.size(), 1u);
    EXPECT_NEAR(intervals[0].start(), 0.3 - halfWidth, 1e-6);
    EXPECT_NEAR(intervals[0].stop(), 0.3 + halfWidth, 1e-6);
}

// 凹陷藏在首段内：采样点 0 与采样点 3 都在事件区内，但两者之间有一段需要排除的事件外区间
// 事件区为 [0, 1.5 - 0.3*sqrt(0.4)] 与 [1.5 + 0.3*sqrt(0.4), 10]
TEST(EventLocatorTest, DipInsideHeadStep)
{
    MockScalarFunc func([](double x) {
        double d = (x - 1.5) / 0.3;
        return d * d - 0.4;
    });
    FixedStepStepper stepper(3.0);
    Interval interval{0.0, 10.0};
    IntervalList intervals;

    aFindEventIntervals(func, stepper, interval, intervals);

    const double halfWidth = 0.3 * std::sqrt(0.4);
    ASSERT_EQ(intervals.size(), 2u);
    EXPECT_DOUBLE_EQ(intervals[0].start(), 0.0);
    EXPECT_NEAR(intervals[0].stop(), 1.5 - halfWidth, 1e-6);
    EXPECT_NEAR(intervals[1].start(), 1.5 + halfWidth, 1e-6);
    EXPECT_DOUBLE_EQ(intervals[1].stop(), 10.0);
}

GTEST_MAIN()
