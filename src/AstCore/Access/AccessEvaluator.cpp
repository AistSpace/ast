///
/// @file      AccessEvaluator.cpp
/// @brief     访问评估器实现
/// @details   步进采样 → 符号翻转检测 → Brent/二分法求根精化 → 区间合并
/// @author    axel
/// @date      2026-07-22
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

#include "AccessEvaluator.hpp"
#include "AccessStepper.hpp"
#include "AstCore/AccessConstraint.hpp"
#include "AstCore/EventLocator.hpp"
#include "AstCore/Stepper.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN


/// @brief 把访问约束适配成事件定位所需的一元标量函数
/// @details 约束以绝对时间点为自变量，EventLocator 以相对秒为自变量，
///          故以 epoch 为零点平移：f(x) = constraint->evaluate(epoch + x)
class AccessConstraintFunc : public UnaryScalarFunc
{
public:
    AccessConstraintFunc(const AccessConstraint& constraint, const TimePoint& epoch)
        : constraint_(constraint)
        , epoch_(epoch)
    {}

    void evaluate(double x, double* out) const override
    {
        *out = constraint_.evaluate(epoch_ + x);
    }

private:
    const AccessConstraint& constraint_;
    TimePoint               epoch_;
};


/// @brief 把访问步进策略适配成事件定位所需的步进器
/// @details 采样时刻仍由 AccessStepper 在绝对时间域上产生，这里以 epoch 为
///          零点换算成相对秒交给 EventLocator
class AccessStepperAdapter : public Stepper
{
public:
    AccessStepperAdapter(AccessStepper& stepper, const TimePoint& epoch)
        : stepper_(stepper)
        , epoch_(epoch)
    {}

    void init(double start, double end) override
    {
        stepper_.init(epoch_ + start, epoch_ + end);
    }

    bool first(double& t) override
    {
        TimePoint tp;
        bool success = stepper_.first(tp);
        t = tp - epoch_;
        return success;
    }

    bool next(double& t) override
    {
        TimePoint tp;
        bool success = stepper_.next(tp);
        t = tp - epoch_;
        return success;
    }

private:
    AccessStepper& stepper_;
    TimePoint      epoch_;
};


/// @brief 评估访问约束的时间段，累加结果到 result 中
/// @details 以区间起点为相对时间零点，把绝对时间域上的约束和步进策略折算到
///          EventLocator 的相对秒域，交由 aFindEventIntervals 完成采样、事件边界
///          精化与步内极值检测，最后把相对区间换算回绝对时间追加到 result
/// @param constraint 访问约束
/// @param stepper 步进策略
/// @param interval 搜索时间区间
/// @param result 满足约束的时间段列表，会累加结果，不会清空已有时段
/// @return 错误码
static errc_t aEvaluateAccess_Append(
    const AccessConstraint* constraint,
    AccessStepper* stepper,
    const TimeInterval& interval,
    TimeIntervalList& result
)
{
    if (!constraint || !stepper) { return eErrorInvalidParam; }

    if(result.empty())
        result.setEpoch(TimePoint{interval.start().integerPart(), 0});
    
    const TimePoint& epoch = result.epoch();
    
    AccessConstraintFunc func(*constraint, epoch);
    AccessStepperAdapter adapter(*stepper, epoch);

    /*
    因为统一设置了 epoch
    这里可以调用 _aFindEventIntervals_Append 函数，直接将结果累加到 result.intervals() 中
    */
    return _aFindEventIntervals_Append(func, adapter, interval.toInterval(epoch), result.intervals());
}


errc_t aEvaluateAccess(
    const AccessConstraint* constraint,
    AccessStepper* stepper,
    const TimeInterval& interval,
    TimeIntervalList& result
)
{
    result.clear();
    return aEvaluateAccess_Append(constraint, stepper, interval, result);
}


errc_t aEvaluateAccess(
    const AccessConstraint* constraint,
    AccessStepper* stepper,
    const TimeIntervalList& intervalList,
    TimeIntervalList& result
)
{
    // 采用函数局部变量存储结果，避免intervalList和result的地址相同导致的异常行为
    TimeIntervalList satisfiedIntervalList;
    errc_t rc = eNoError;
    for (const auto& interval : intervalList) {
        errc_t err = aEvaluateAccess_Append(constraint, stepper, interval, satisfiedIntervalList);
        if (err != eNoError) {
            rc = err;
            aError(_("计算可见性失败，区间：%s"), interval.toString().c_str());
        }
    }
    result = std::move(satisfiedIntervalList);
    return rc;
}


errc_t aEvaluateAccess(
    const std::vector<AccessConstraint*>& constraints,
    AccessStepper* stepper,
    const TimeInterval& interval,
    TimeIntervalList& result
)
{
    if(constraints.empty())
    {
        // 没有约束，直接返回输入的时间区间
        result = TimeIntervalList::FromTimeInterval(interval);
        aInfo(_("无约束，返回输入区间：%s"), interval.toString().c_str());
        return eNoError;
    }
    errc_t rc = aEvaluateAccess(constraints[0], stepper, interval, result);
    for(size_t i=1; i<constraints.size(); i++)
    {
        rc |= aEvaluateAccess(constraints[i], stepper, result, result);
    }
    return rc;
}


AccessEvaluator::AccessEvaluator()  = default;
AccessEvaluator::~AccessEvaluator() = default;

bool AccessEvaluator::check(const TimePoint& time) const
{
    if (!constraint_) { return false; }
    return constraint_->evaluate(time) >= 0.0;
}

errc_t AccessEvaluator::evaluate(const TimeInterval& interval, TimeIntervalList& result)
{
    return aEvaluateAccess(constraint_, stepper_, interval, result);
}

AST_NAMESPACE_END
