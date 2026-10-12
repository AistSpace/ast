///
/// @file      EventLocator.cpp
/// @brief     事件区间定位实现
/// @details   步进采样 → 符号翻转检测（含由采样斜率反向发现的步内局部极值穿越）→ Brent/二分法求根精化 → 输出非负区间
/// @author    axel
/// @date      2026-10-11
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

#include "EventLocator.hpp"
#include "Stepper.hpp"
#include "AstMath/BrentOptimizer.hpp"
#include "AstMath/BrentSolver.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN


constexpr double kEventZeroTol = 1e-9;  // 函数值零容差：浮点噪声可能产生 ~1e-10 偏差
constexpr double kEventTimeTol = 1e-4;  // 求根的时间精度[s]

/// @brief 判断函数值是否满足事件条件（非负，容忍浮点噪声）
static inline bool isEventSatisfied(double v) { return v >= -kEventZeroTol; }

/// @brief 带容差地判断相邻采样值增量的符号（+1 递增 / -1 递减 / 0 平台）
static inline int slopeSign(double dv)
{
    if (dv >  kEventZeroTol) { return 1; }
    if (dv < -kEventZeroTol) { return -1; }
    return 0;
}


/// @brief 计算一元标量函数值
static double evaluateEventFunc(UnaryScalarFunc& func, double x)
{
    double value = 0.0;
    func.evaluate(x, &value);
    return value;
}


/// @brief 精化单步内的一次事件边界（函数变号处）
/// @details 要求步两端的事件状态不同（isEventSatisfied 结果相反）。
///          一端函数值近零（平台状）时 Brent 无法精化，改用二分法。
/// @param func 一元标量函数
/// @param t0 步起点
/// @param f0 t0 处的函数值
/// @param t1 步终点
/// @param f1 t1 处的函数值
/// @return 事件边界时刻，位于 [t0, t1] 内
static double locateBoundary(UnaryScalarFunc& func, double t0, double f0, double t1, double f1)
{
    const double dt = t1 - t0;
    double rootT = 0.0;

    if (fabs(f0) <= kEventZeroTol || fabs(f1) <= kEventZeroTol) {
        // 一端近零（平台状）：Brent 无法精化，改用二分法
        // neg: 不满足端, zero: 满足端
        double neg, zero;
        if (!isEventSatisfied(f0)) {
            neg = 0.0;  zero = dt;
        } else {
            neg = dt;   zero = 0.0;
        }
        while (fabs(zero - neg) > kEventTimeTol) {
            double mid = (neg + zero) * 0.5;
            if (isEventSatisfied(evaluateEventFunc(func, t0 + mid))) {
                zero = mid;
            } else {
                neg = mid;
            }
        }
        rootT = zero; // 取满足端
    } else {
        // 两端都非零：正常 Brent 求根
        BrentSolver().solve([&func, t0](double x) -> double {
            return evaluateEventFunc(func, t0 + x);
        }, 0.0, dt, rootT);
    }

    return t0 + rootT;
}


/// @brief 在给定括号内检测因局部极值引起的事件穿越
/// @details 括号两端事件状态相同，但括号内存在极值点跨越零：非事件区间内藏正的凸起（负→负却有区间），
///          或事件区间内藏负的凹陷（正→正却有区间需要排除）。此时括号内恰有两次穿越。
///          括号取自采样斜率反向处，通常覆盖极值所在的两个步长。
/// @param func 一元标量函数
/// @param t0 括号左端
/// @param f0 t0 处的函数值
/// @param t1 括号右端
/// @param f1 t1 处的函数值
/// @param inEvent t0 处是否处于事件中
/// @param[out] first 第一次穿越（进入事件）的时刻
/// @param[out] second 第二次穿越（离开事件）的时刻
/// @return 括号内存在两次穿越时返回 true
/// @note 仅在括号内极值唯一（单峰）时可靠；括号内存在多个极值时只会检查到其中一个
static bool findInteriorBoundaries(
    UnaryScalarFunc& func, double t0, double f0, double t1, double f1,
    bool inEvent, double& first, double& second)
{
    // 非事件括号求极大值（对 -f 求极小），事件括号求极小值
    const bool wantMax = !inEvent;
    double extTime  = t0;
    double extValue = 0.0;
    aBrentMinimize([&func, wantMax](double x) -> double {
        double v = evaluateEventFunc(func, x);
        return wantMax ? -v : v;
    }, t0, t1, extTime, &extValue);
    if (wantMax) { extValue = -extValue; }

    // 极值点未跨出原状态（负括号内仍为负 / 正括号内仍为正）：括号内无事件
    if (isEventSatisfied(extValue) == inEvent) { return false; }

    // 括号内两次穿越：t0 → 极值点，极值点 → t1
    first  = locateBoundary(func, t0, f0, extTime, extValue);
    second = locateBoundary(func, extTime, extValue, t1, f1);

    // 极值只是相切于零、并未真正穿出时（函数在极值处不连续时，取到的还可能只是其中一侧的极限值），
    // 两次定位会落在同一时刻上。零宽区间没有意义
    if (!(second - first > kEventTimeTol)) { return false; }

    return true;
}


errc_t _aFindEventIntervals_Append(
    UnaryScalarFunc& func,
    Stepper& stepper,
    const Interval& interval,
    IntervalList& intervals
)
{
    // 空区间（反向区间或含 NaN）直接返回空结果，同时避免步进器陷入死循环
    if (interval.isEmpty()) { return eNoError; }

    double prevTime;
    double prevValue;
    int    prevDir    = 0;          // 最近一次非零斜率的方向

    // 初始化步进器并取第一个采样点
    stepper.init(interval);
    if (!stepper.first(prevTime)) { return eNoError; }  // 区间内无采样点
    prevValue = evaluateEventFunc(func, prevTime);

    // 当前是否处于事件区间内
    bool   inEventInterval = isEventSatisfied(prevValue);
    // 当前事件区间开始时间
    double eventIntervalStart = prevTime;

    // 采样历史：由相邻两步的斜率反向廉价地发现步内局部极值，
    // 正常情况下不产生任何额外函数求值，仅在极值可能跨零时才做极小化精化
    double prev2Time  = prevTime;   // 上上个样本
    double prev2Value = prevValue;

    double currentTime;

    while (stepper.next(currentTime)) 
    {
        const double currentValue = evaluateEventFunc(func, currentTime);
        const int currentDir = slopeSign(currentValue - prevValue);

        // 情况1: 符号翻转（使用容差，容忍浮点噪声导致函数值不精确为零）
        if (isEventSatisfied(prevValue) != isEventSatisfied(currentValue)) {
            // 此时该步内有一次事件边界
            double rootTime = locateBoundary(func, prevTime, prevValue, currentTime, currentValue);

            if (inEventInterval) {
                intervals.push_back(eventIntervalStart, rootTime);
            }

            eventIntervalStart = rootTime;
            inEventInterval = !inEventInterval;
        }
        // 情况2: 步内可能存在极值穿越（当前步进前后函数值不变，或与上次步进时的斜率方向相反）
        else if(prevDir != 0 && currentDir != prevDir)
        {
            /*
            @note
            这里使用 prevDir != 0 来进行筛选
            避免了在步进区间的两个端点函数值相同时，该部分区间进入该逻辑两次，从而产生一个非法的反向区间
            同时，通过判断 prevDir != 0 避免了第一步时进入该逻辑（因为 prevDir 变量初始化为 0）
            */
            
            // 极大值（增转减）只在非事件区间内可能向上穿出；
            // 极小值（减转增）只在事件区间内可能向下穿出。其余情形极值不会跨零，无需精化
            const bool mayCross = (prevDir > 0) ? !inEventInterval : inEventInterval;
            if (mayCross) {
                double first, second;
                if (findInteriorBoundaries(func, prev2Time, prev2Value, currentTime, currentValue,
                                        inEventInterval, first, second)) {
                    if (inEventInterval) {
                        // 事件区间被凹陷打断：先前区间止于 first，新区间自 second 起
                        intervals.push_back(eventIntervalStart, first);
                        eventIntervalStart = second;
                        // inEventInterval = true;
                    } else {
                        // 存在 >0 的凸起：输出 [first, second] 这段独立区间
                        intervals.push_back(first, second);
                        // inEventInterval = false;
                    }
                }
            }
        }

        prev2Time  = prevTime;
        prev2Value = prevValue;
        prevTime   = currentTime;
        prevValue  = currentValue;
        prevDir    = currentDir;
    }

    // 处理最后一个区间
    if (inEventInterval) {
        intervals.push_back(eventIntervalStart, interval.stop());
    }

    return eNoError;
}

errc_t aFindEventIntervals(
    UnaryScalarFunc& func,
    Stepper& stepper,
    const Interval& interval,
    IntervalList& intervals
)
{
    intervals.clear();
    return _aFindEventIntervals_Append(func, stepper, interval, intervals);
}

AST_NAMESPACE_END
