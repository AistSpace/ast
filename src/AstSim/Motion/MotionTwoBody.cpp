///
/// @file      MotionTwoBody.cpp
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

#include "MotionTwoBody.hpp"
#include "AstCore/EphemerisTwoBody.hpp"
#include "AstCore/EphemerisLagrangeFixed.hpp"
#include "AstCore/EphemerisLagrangeVar.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/TwoBody.hpp"

AST_NAMESPACE_BEGIN

MotionTwoBody *MotionTwoBody::New()
{
    return new MotionTwoBody();
}

#define AST_CHECK_NULLPTR(obj) if(obj == nullptr){aError(#obj " is nullptr"); return eErrorNullPtr;}
#define AST_CHECK_ERRCODE(rc, msg) if(rc){aError(msg); return rc;}

err_t MotionTwoBody::makeEphemerisSpec(ScopedPtr<Ephemeris> &eph) const
{
    TimePoint epoch{};
    auto initstate = this->getInitialState(); AST_CHECK_NULLPTR(initstate);

    err_t rc = initstate->getStateEpoch(epoch); AST_CHECK_ERRCODE(rc, "failed to get initial epoch");

    auto propFrame = this->getPropagationFrame(); AST_CHECK_NULLPTR(propFrame);

    CartState cartState;
    rc = initstate->getStateIn(propFrame, cartState); AST_CHECK_ERRCODE(rc, "failed to get initial state");
    
    auto ephTwoBody = EphemerisTwoBody::New(propFrame, epoch, cartState);
    eph = ephTwoBody;
    return eNoError;
}

err_t MotionTwoBody::makeEphemerisSimple(ScopedPtr<Ephemeris> &eph) const
{
    TimeInterval interval;
    this->getInterval(interval);
    auto frame = this->getPropagationFrame();
    if(frame == nullptr){
        // aWarning("propagation frame is nullptr, use body's initial frame as the default frame");
        // frame = this->getInitialState()->getFrame()->getBody()->makeFrameInertial();
        aError("propagation frame is nullptr");
        return eErrorNullPtr;
    }

    return err_t();
}

AST_NAMESPACE_END


