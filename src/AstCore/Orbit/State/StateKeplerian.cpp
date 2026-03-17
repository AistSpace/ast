///
/// @file      StateKeplerian.cpp
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

#include "StateKeplerian.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstUtil/Logger.hpp"
#include <climits>

AST_NAMESPACE_BEGIN

PStateKeplerian StateKeplerian::New()
{
    return new StateKeplerian();
}

HStateKeplerian StateKeplerian::MakeShared()
{
    return new StateKeplerian();
}

PStateKeplerian StateKeplerian::New(const ModOrbElem &modOrbElem)
{
    return new StateKeplerian(modOrbElem);
}

HStateKeplerian StateKeplerian::MakeShared(const ModOrbElem &modOrbElem)
{
    return new StateKeplerian(modOrbElem);
}

StateKeplerian::StateKeplerian(const ModOrbElem &modOrbElem)
    : modOrbElem_(modOrbElem)
{
}

StateKeplerian::StateKeplerian(const State &state)
    : State{state}
{
    if (state.getStateType() == EStateType::eKeplerian)
    {
        static_cast<const StateKeplerian&>(state).getState(modOrbElem_);
    }else{
        CartState cartState;
        err_t rc = state.getState(cartState);
        if(rc) return;
        this->setState(cartState);
    }
}

err_t StateKeplerian::getState(CartState &state) const
{
    return aModOrbElemToCart(modOrbElem_, getGM(), state.pos(), state.vel());
}

err_t StateKeplerian::setState(const CartState &state)
{
    return aCartToModOrbElem(state.pos(), state.vel(), getGM(), modOrbElem_);
}

void StateKeplerian::setState(OrbElem &orbElem)
{
    return coe2moe(orbElem.data(), modOrbElem_.data());
}

void StateKeplerian::getState(OrbElem &orbElem) const
{
    err_t rc = moe2coe(modOrbElem_.data(), orbElem.data());
    A_UNUSED(rc);
}

void StateKeplerian::getInnerRepresentationState(array6d &stateInRepresentation) const
{
    return getStateInRepresentation(
        this->sizeType_, this->shapeType_, 
        this->orientationType_, this->positionType_, 
        stateInRepresentation
    );
}

void StateKeplerian::getStateInRepresentation(
    ESizeType sizeType, EShapeType shapeType, 
    EOrientationType orientationType, EPositionType positionType, 
    array6d &stateInRepresentation) const
{
    stateInRepresentation[0] = getSizeParam(sizeType);
    stateInRepresentation[1] = getShapeParam(shapeType);
    stateInRepresentation[2] = getInc();
    stateInRepresentation[3] = getOrientationParam(orientationType);
    stateInRepresentation[4] = getArgPeri();
    stateInRepresentation[5] = getPositionParam(positionType);
}

double StateKeplerian::getStateParam(int index)
{
    switch (index)
    {
    case 0:
        return getSizeParam();
    case 1:
        return getShapeParam();
    case 2:
        return getInc();
    case 3:
        return getOrientationParam();
    case 4:
        return getArgPeri();
    case 5:
        return getPositionParam();
    default:
        break;
    }
    return std::numeric_limits<double>::quiet_NaN();
}

void StateKeplerian::setStateParam(int index, double value)
{
    switch (index)
    {
    case 0:
        setSizeParam(value);
        break;
    case 1:
        setShapeParam(value);
        break;
    case 2:
        setInc(value);
        break;
    case 3:
        setOrientationParam(value);
        break;
    case 4:
        setArgPeri(value);
        break;
    case 5:
        setPositionParam(value);
        break;
    default:
        break;
    }
}

double StateKeplerian::getSizeParam() const
{
    return getSizeParam(this->sizeType_);
}

double StateKeplerian::getSizeParam(ESizeType sizeType) const
{
    switch (sizeType)
    {
    case ESizeType::eSMA:
        return this->getSMA();
    case ESizeType::eApoAlt:
        return this->getApoAlt();
    case ESizeType::eApoRad:
        return this->getApoRad();
    case ESizeType::ePeriAlt:
        return this->getPeriAlt();
    case ESizeType::ePeriRad:
        return this->getPeriRad();
    case ESizeType::ePeriod:
        return this->getPeriod();
    case ESizeType::eMeanMotion:
        return this->getMeanMotion();
    default:
        aError("unknown sizeType");
        return std::numeric_limits<double>::quiet_NaN();
    }
}

void StateKeplerian::setSizeParam(double sizeParam, ESizeType sizeType)
{
    switch (sizeType)
    {
    case ESizeType::eSMA:
        this->setSMA(sizeParam);
        break;
    case ESizeType::eApoAlt:
        this->setApoAltForSize(sizeParam);
        break;
    case ESizeType::eApoRad:
        this->setApoRadForSize(sizeParam);
        break;
    case ESizeType::ePeriAlt:
        this->setPeriAltForSize(sizeParam);
        break;
    case ESizeType::ePeriRad:
        this->setPeriRadForSize(sizeParam);
        break;
    case ESizeType::ePeriod:
        this->setPeriod(sizeParam);
        break;
    case ESizeType::eMeanMotion:
        this->setMeanMotion(sizeParam);
        break;
    default:
        aError("unknown sizeType");
        break;
    }
}

void StateKeplerian::setSizeParam(double sizeParam)
{
    this->setSizeParam(sizeParam, this->sizeType_);
}

void StateKeplerian::setSizeType(ESizeType sizeType)
{
    this->sizeType_ = sizeType;
}

double StateKeplerian::getShapeParam() const
{
    return getShapeParam(this->shapeType_);
}

double StateKeplerian::getShapeParam(EShapeType shapeType) const
{
    switch (shapeType)
    {
    case EShapeType::eApoAlt:
        return this->getApoAlt();
    case EShapeType::ePeriAlt:
        return this->getPeriAlt();
    case EShapeType::eEcc:
        return this->getEcc();
    case EShapeType::eApoRad:
        return this->getApoRad();
    case EShapeType::ePeriRad:
        return this->getPeriRad();
    default:
        aError("unknown shapeType");
        return std::numeric_limits<double>::quiet_NaN();
    }
}

void StateKeplerian::setShapeParam(double shapeParam, EShapeType shapeType)
{
    switch (shapeType)
    {
    case EShapeType::eApoAlt:
        this->setApoAltForShape(shapeParam);
        break;
    case EShapeType::ePeriAlt:
        this->setPeriAltForShape(shapeParam);
        break;
    case EShapeType::eEcc:
        this->setEcc(shapeParam);
        break;
    case EShapeType::eApoRad:
        this->setApoRadForShape(shapeParam);
        break;
    case EShapeType::ePeriRad:
        this->setPeriRadForShape(shapeParam);
        break;
    default:
        aError("unknown shapeType");
        break;
    }
}

void StateKeplerian::setShapeParam(double shapeParam)
{
    this->setShapeParam(shapeParam, this->shapeType_);
}

void StateKeplerian::setShapeType(EShapeType shapeType)
{
    this->shapeType_ = shapeType;
}

double StateKeplerian::getOrientationParam() const
{
    return getOrientationParam(this->orientationType_);
}

double StateKeplerian::getOrientationParam(EOrientationType orientationType) const
{
    switch (orientationType)
    {
    case EOrientationType::eRAAN:
        return this->getRAAN();
    case EOrientationType::eLAN:
        return this->getLAN();
    default:
        aError("unknown orientationType");
        return std::numeric_limits<double>::quiet_NaN();
    }
}

void StateKeplerian::setOrientationParam(double orientationParam, EOrientationType orientationType)
{
    switch (orientationType)
    {
    case EOrientationType::eRAAN:
        this->setRAAN(orientationParam);
        break;
    case EOrientationType::eLAN:
        this->setLAN(orientationParam);
        break;
    default:
        aError("unknown orientationType");
        break;
    }
}

void StateKeplerian::setOrientationParam(double orientationParam)
{
    this->setOrientationParam(orientationParam, this->orientationType_);
}

void StateKeplerian::setOrientationType(EOrientationType orientationType)
{
    this->orientationType_ = orientationType;
}

double StateKeplerian::getPositionParam() const
{
    return getPositionParam(this->positionType_);
}

double StateKeplerian::getPositionParam(EPositionType positionType) const
{
    switch (positionType)
    {
    case EPositionType::eTrueAnomaly:
        return this->getTrueAnomaly();
    case EPositionType::eMeanAnomaly:
        return this->getMeanAnomaly();
    case EPositionType::eEccAnomaly:
        return this->getEccAnomaly();
    case EPositionType::eArgLat:
        return this->getArgLat();
    case EPositionType::eTimePastAscNode:
        return this->getTimePastAscNode();
    case EPositionType::eTimePastPeri:
        return this->getTimePastPeri();
    case EPositionType::eTimeOfAscNodePassage:
        return this->getTimeOfAscNodePassage();
    case EPositionType::eTimeOfPeriPassage:
        return this->getTimeOfPeriPassage();
    default:
        aError("unknown positionType");
        return std::numeric_limits<double>::quiet_NaN();
    }
}

void StateKeplerian::setPositionParam(double positionParam, EPositionType positionType)
{
    switch (positionType)
    {
    case EPositionType::eTrueAnomaly:
        this->setTrueAnomaly(positionParam);
        break;
    case EPositionType::eMeanAnomaly:
        this->setMeanAnomaly(positionParam);
        break;
    case EPositionType::eEccAnomaly:
        this->setEccAnomaly(positionParam);
        break;
    case EPositionType::eArgLat:
        this->setArgLat(positionParam);
        break;
    case EPositionType::eTimePastAscNode:
        this->setTimePastAscNode(positionParam);
        break;
    case EPositionType::eTimePastPeri:
        this->setTimePastPeri(positionParam);
        break;
    case EPositionType::eTimeOfAscNodePassage:
        this->setTimeOfAscNodePassage(positionParam);
        break;
    case EPositionType::eTimeOfPeriPassage:
        this->setTimeOfPeriPassage(positionParam);
        break;
    default:
        aError("unknown positionType");
        break;
    }
}

void StateKeplerian::setPositionParam(double positionParam)
{
    this->setPositionParam(positionParam, this->positionType_);
}

void StateKeplerian::setPositionType(EPositionType positionType)
{
    this->positionType_ = positionType;
}

void StateKeplerian::setSMA(double sma)
{
    aError("setSMA not implemented");
}

double StateKeplerian::getMeanMotion() const
{
    return modOrbElem_.getMeanMotion(getGM());
}

void StateKeplerian::setMeanMotion(double meanMotion)
{
    aError("setMeanMotion not implemented");
}

double StateKeplerian::getPeriod() const
{
    return modOrbElem_.getPeriod(getGM());
}

void StateKeplerian::setPeriod(double period)
{
    aError("setPeriod not implemented");
}

double StateKeplerian::getApoAlt() const
{
    return modOrbElem_.getApoAlt(getBodyRadius());
}

void StateKeplerian::setApoAltForSize(double apoAlt)
{
    aError("setApoAltForSize not implemented");
}

void StateKeplerian::setApoAltForShape(double apoAlt)
{
    aError("setApoAltForShape not implemented");
}

double StateKeplerian::getApoRad() const
{
    return modOrbElem_.getApoRad();
}

void StateKeplerian::setApoRadForSize(double apoRad)
{
    aError("setApoRadForSize not implemented");
}

void StateKeplerian::setApoRadForShape(double apoRad)
{
    aError("setApoRadForShape not implemented");
}

double StateKeplerian::getPeriAlt() const
{
    return modOrbElem_.getPeriAlt(getBodyRadius());
}

void StateKeplerian::setPeriAltForSize(double periAlt)
{
    aError("setPeriAltForSize not implemented");
}

void StateKeplerian::setPeriAltForShape(double periAlt)
{
    aError("setPeriAltForShape not implemented");
}

double StateKeplerian::getPeriRad() const
{
    return modOrbElem_.getPeriRad();
}

void StateKeplerian::setPeriRadForSize(double periRad)
{
    aError("setPeriRadForSize not implemented");
}

void StateKeplerian::setPeriRadForShape(double periRad)
{
    aError("setPeriRadForShape not implemented");
}

double StateKeplerian::getEcc() const
{
    return modOrbElem_.getEcc();
}

void StateKeplerian::setEcc(double ecc)
{
    aError("setEcc not implemented");
}

double StateKeplerian::getInc() const
{
    return modOrbElem_.getInc();
}

void StateKeplerian::setInc(double inc)
{
    aError("setInc not implemented");
}

double StateKeplerian::getRAAN() const
{
    return modOrbElem_.getRAAN();
}

void StateKeplerian::setRAAN(double raan)
{
    aError("setRAAN not implemented");
}

double StateKeplerian::getLAN() const
{
    auto body = this->getBody();
    if(!body)
    {
        aError("failed to get body");
        return std::numeric_limits<double>::quiet_NaN();
    }

    TimePoint timeOfAscNodePassage;
    this->getTimeOfAscNodePassage(timeOfAscNodePassage);
    double raan = modOrbElem_.getRAAN();
    return aRAANToLAN(raan, frame_->getAxes(), timeOfAscNodePassage, body->getAxesFixed());
}

void StateKeplerian::setLAN(double lan)
{
    aError("setLAN not implemented");
}

double StateKeplerian::getSMA() const
{
    return modOrbElem_.getSMA();
}

double StateKeplerian::getArgPeri() const
{
    return modOrbElem_.getArgPeri();
}

void StateKeplerian::setArgPeri(double argPeri)
{
    aError("setArgPeri not implemented");
}

double StateKeplerian::getTrueAnomaly() const
{
    return modOrbElem_.getTrueAnomaly();
}

void StateKeplerian::setTrueAnomaly(double trueAnomaly)
{
    aError("setTrueAnomaly not implemented");
}

double StateKeplerian::getMeanAnomaly() const
{
    return modOrbElem_.getMeanAnomaly();
}

void StateKeplerian::setMeanAnomaly(double meanAnomaly)
{
    aError("setMeanAnomaly not implemented");
}

double StateKeplerian::getEccAnomaly() const
{
    return modOrbElem_.getEccAnomaly();
}

void StateKeplerian::setEccAnomaly(double eccAnomaly)
{
    aError("setEccAnomaly not implemented");
}

double StateKeplerian::getArgLat() const
{
    return modOrbElem_.getArgLat();
}

void StateKeplerian::setArgLat(double argLat)
{
    aError("setArgLat not implemented");
}

double StateKeplerian::getTimePastPeri() const
{
    return modOrbElem_.getTimePastPeri(getGM());
}

void StateKeplerian::setTimePastPeri(double timePastPeri)
{
    aError("setTimePastPeri not implemented");
}

double StateKeplerian::getTimePastAscNode() const
{
    return modOrbElem_.getTimePastAscNode(getGM());
}

void StateKeplerian::setTimePastAscNode(double timePastAscNode)
{
    aError("setTimePastAscNode not implemented");
}

void StateKeplerian::getTimeOfPeriPassage(TimePoint &tp) const
{
    TimePoint stateEpoch;
    err_t rc = this->getStateEpoch(stateEpoch);
    A_UNUSED(rc);
    tp = stateEpoch - this->getTimePastPeri();
}

double StateKeplerian::getTimeOfPeriPassage() const
{
    TimePoint tp;
    this->getTimeOfPeriPassage(tp);
    return aTimePointToEpochSecond(tp);
}

void StateKeplerian::setTimeOfPeriPassage(const TimePoint &tp)
{
    aError("setTimeOfPeriPassage not implemented");
}

void StateKeplerian::setTimeOfPeriPassage(double epochsecond)
{
    aError("setTimeOfPeriPassage not implemented");
}

void StateKeplerian::getTimeOfAscNodePassage(TimePoint &tp) const
{
    TimePoint stateEpoch;
    err_t rc = this->getStateEpoch(stateEpoch);
    A_UNUSED(rc);
    tp = stateEpoch - this->getTimePastAscNode();
}


double StateKeplerian::getTimeOfAscNodePassage() const
{
    TimePoint tp;
    this->getTimeOfAscNodePassage(tp);
    return aTimePointToEpochSecond(tp);
}

void StateKeplerian::setTimeOfAscNodePassage(const TimePoint &tp)
{
    aError("setTimeOfAscNodePassage not implemented");
}

void StateKeplerian::setTimeOfAscNodePassage(double epochsecond)
{
    TimePoint tp;
    aEpochSecondToTimePoint(epochsecond, tp);
    this->setTimeOfAscNodePassage(tp);
}

AST_NAMESPACE_END


