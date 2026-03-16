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
#include "AstCore/FrameAssembly.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstMath/KinematicTransform.hpp"


AST_NAMESPACE_BEGIN

void State::setFrame(Frame *frame)
{
    frame_ = frame;
    gm_ = frame_->getGM();
}

err_t State::changeFrame(Frame *frame)
{
    if(frame_ == frame)
        return eNoError;
    CartState state;
    this->getState(state);
    KinematicTransform transform;
    TimePoint epoch;
    err_t rc = getStateEpoch(epoch);
    if(rc) return rc;
    rc = aFrameTransform(this->frame_, frame, epoch, transform);
    if(rc) return rc;
    state = transform.transformPositionVelocity(state);
    this->setState(state);
    this->setFrame(frame);
    return eNoError;
}

void State::setStateEpoch(EventTime *stateEpoch)
{
    stateEpoch_ = stateEpoch;
}



void State::setStateEpoch(const TimePoint &stateEpoch)
{
    stateEpoch_ = EventTimeExplicit::New(stateEpoch);
}

err_t State::getStateEpoch(TimePoint &stateEpoch) const
{
    return stateEpoch_->getTime(stateEpoch);
}

CelestialBody *State::getBody() const
{
    if(frame_)
        return frame_->getBody();
    return nullptr;
}

double State::getBodyRadius() const
{
    if(auto body = getBody())
        return body->getRadius();
    return 0.0;
}

#if 0 

void State::setCoordEpoch(const TimePoint &coordEpoch)
{
    return setCoordEpoch(EventTimeExplicit::New(coordEpoch));
}

void State::setCoordEpoch(EventTime *coordEpoch)
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        framewithepoch->setEpoch(coordEpoch);
    }else{
        auto point = frame_->getOrigin();
        auto axes = frame_->getAxes();
        auto frozenFrame = FrameWithEpoch::MakeShared(point, axes, coordEpoch);
        this->setFrame(frozenFrame.get());
    }
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
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        return framewithepoch->getUseEpoch();
    }
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
        auto frozenFrame = FrameWithEpoch::MakeShared(point, axes, stateEpoch_);
        this->setFrame(frozenFrame.get());
    }
}

void State::setCoordAxes(Axes *axes)
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        framewithepoch->setSourceAxes(axes);
    }else{
        auto point = frame_->getOrigin();
        auto newframe = FrameAssembly::MakeShared(point, axes);
        this->setFrame(newframe.get());
    }
}

err_t State::changeCoordAxes(Axes *axes)
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        auto newframe = FrameWithEpoch::MakeShared(frame_->getOrigin(), axes, framewithepoch->getEpoch());
        newframe->setUseEpoch(framewithepoch->getUseEpoch());
        return changeFrame(newframe.get());
    }else{
        auto point = frame_->getOrigin();
        auto newframe = FrameAssembly::MakeShared(point, axes);
        return changeFrame(newframe.get());
    }
}

Axes *State::getCoordAxes() const
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        return framewithepoch->getSourceAxes();
    }
    return frame_->getAxes();
}
#endif

AST_NAMESPACE_END
