///
/// @file      State.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
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

#include "State.hpp"
#include "AstCore/EventTimeExplicit.hpp"
#include "AstCore/FrameWithEpoch.hpp"

AST_NAMESPACE_BEGIN

err_t State::changeFrame(Frame *frame)
{
    if(frame_ == frame)
        return eNoError;
    frame_ = frame;
    return eNoError;
}

void State::setStateEpoch(EventTime *stateEpoch)
{
    stateEpoch_ = stateEpoch;
}

void State::setCoordEpoch(EventTime *coordEpoch)
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        framewithepoch->setEpoch(coordEpoch);
    }else{
        auto point = frame_->getOrigin();
        auto axes = frame_->getAxes();
        auto frozenAxes = FrameWithEpoch::MakeShared(point, axes, coordEpoch);
        frame_ = frozenAxes;
    }
}

void State::setStateEpoch(const TimePoint &stateEpoch)
{
    stateEpoch_ = EventTimeExplicit::New(stateEpoch);
}

void State::setCoordEpoch(const TimePoint &coordEpoch)
{
    return setCoordEpoch(EventTimeExplicit::New(coordEpoch));
}

err_t State::getStateEpoch(TimePoint &stateEpoch) const
{
    return stateEpoch_->getTime(stateEpoch);
}

err_t State::getCoordEpoch(TimePoint &coordEpoch) const
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        return framewithepoch->getEpoch(coordEpoch);
    }
    return eErrorNullPtr;
}

bool State::getUseCoordEpoch() const
{
    return false;
}

void State::setUseCoordEpoch(bool useCoordEpoch)
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        framewithepoch->setUseEpoch(useCoordEpoch);
    }else{
        auto point = frame_->getOrigin();
        auto axes = frame_->getAxes();
        auto frozenAxes = FrameWithEpoch::MakeShared(point, axes, stateEpoch_);
        frame_ = frozenAxes;
    }
}

AST_NAMESPACE_END
