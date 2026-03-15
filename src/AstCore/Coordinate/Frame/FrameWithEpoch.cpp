///
/// @file      FrameWithEpoch.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-15
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "FrameWithEpoch.hpp"



AST_NAMESPACE_BEGIN

FrameWithEpoch *FrameWithEpoch::New(Point *origin, Axes *axes)
{
    return new FrameWithEpoch(origin, axes);
}

FrameWithEpoch *FrameWithEpoch::New(Point *origin, Axes *axes, EventTime *epoch)
{
    return new FrameWithEpoch(origin, axes, epoch);
}

FrameWithEpoch *FrameWithEpoch::MakeShared(Point *origin, Axes *axes)
{
    return new FrameWithEpoch(origin, axes);
}

FrameWithEpoch *FrameWithEpoch::MakeShared(Point *origin, Axes *axes, EventTime *epoch)
{
    return new FrameWithEpoch(origin, axes, epoch);
}

FrameWithEpoch::FrameWithEpoch()
    : axesFrozen_{new AxesFrozenAtEventTime()}
{
}

FrameWithEpoch::FrameWithEpoch(Point *origin, Axes *axes)
    : origin_{origin}, axesFrozen_{new AxesFrozenAtEventTime(axes, nullptr, nullptr)}, axes_{axes}, useEpoch_{false}
{
}

FrameWithEpoch::FrameWithEpoch(Point *origin, Axes *axes, EventTime *epoch)
    : origin_{origin}
    , axesFrozen_{new AxesFrozenAtEventTime(axes, epoch, nullptr)}
    , axes_{axes}
    , useEpoch_{true}
{
}

Axes *FrameWithEpoch::getAxes() const
{
    if(useEpoch_)
        return axesFrozen_.get();
    else
        return axes_.get();
}

Point *FrameWithEpoch::getOrigin() const
{
    return origin_;
}

SharedPtr<EventTime> &FrameWithEpoch::getEpochHandle()
{
    return axesFrozen_->getEpochHandle();
}

EventTime *FrameWithEpoch::getEpoch() const
{
    return axesFrozen_->getEpoch();
}

err_t FrameWithEpoch::getEpoch(TimePoint &epoch) const
{
    return axesFrozen_->getEpoch(epoch);
}

void FrameWithEpoch::setEpoch(EventTime *epoch)
{
    return axesFrozen_->setEpoch(epoch);
}

void FrameWithEpoch::setAxes(Axes *axes)
{
    axes_ = axes;
    axesFrozen_->setAxes(axes);
}

Axes *FrameWithEpoch::getReferenceAxes() const
{
    return axesFrozen_->getReferenceAxes();
}

void FrameWithEpoch::setReferenceAxes(Axes *axes)
{
    axesFrozen_->setReferenceAxes(axes);
}

AST_NAMESPACE_END


