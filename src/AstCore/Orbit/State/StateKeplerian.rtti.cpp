///
/// @file      StateKeplerian.rtti.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-16
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

#include "StateKeplerian.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/ReflectAPI.hpp"

AST_NAMESPACE_BEGIN

void StateKeplerian_ClassInit(Class* cls)
{
    cls->addProperty("SMA",             aNewPropertyDouble<StateKeplerian, &StateKeplerian::getSMA, &StateKeplerian::setSMA>());
    cls->addProperty("Period",          aNewPropertyDouble<StateKeplerian, &StateKeplerian::getPeriod, &StateKeplerian::setPeriod>());
    cls->addProperty("MeanMotion",      aNewPropertyDouble<StateKeplerian, &StateKeplerian::getMeanMotion, &StateKeplerian::setMeanMotion>());
    cls->addProperty("ApoRadForSize",   aNewPropertyDouble<StateKeplerian, &StateKeplerian::getApoRad, &StateKeplerian::setApoRadForSize>());
    cls->addProperty("ApoAltForSize",   aNewPropertyDouble<StateKeplerian, &StateKeplerian::getApoAlt, &StateKeplerian::setApoAltForSize>());
    cls->addProperty("PeriRadForSize",  aNewPropertyDouble<StateKeplerian, &StateKeplerian::getPeriRad, &StateKeplerian::setPeriRadForSize>());
    cls->addProperty("PeriAltForSize",  aNewPropertyDouble<StateKeplerian, &StateKeplerian::getPeriAlt, &StateKeplerian::setPeriAltForSize>());

    cls->addProperty("Ecc",             aNewPropertyDouble<StateKeplerian, &StateKeplerian::getEcc, &StateKeplerian::setEcc>());
    cls->addProperty("ApoRadForShape",  aNewPropertyDouble<StateKeplerian, &StateKeplerian::getApoRad, &StateKeplerian::setApoRadForShape>());
    cls->addProperty("ApoAltForShape", aNewPropertyDouble<StateKeplerian, &StateKeplerian::getApoAlt, &StateKeplerian::setApoAltForShape>());
    cls->addProperty("PeriRadForShape", aNewPropertyDouble<StateKeplerian, &StateKeplerian::getPeriRad, &StateKeplerian::setPeriRadForShape>());
    cls->addProperty("PeriAltForShape", aNewPropertyDouble<StateKeplerian, &StateKeplerian::getPeriAlt, &StateKeplerian::setPeriAltForShape>());

    cls->addProperty("Inc",             aNewPropertyDouble<StateKeplerian, &StateKeplerian::getInc, &StateKeplerian::setInc>());

    cls->addProperty("RAAN",            aNewPropertyDouble<StateKeplerian, &StateKeplerian::getRAAN, &StateKeplerian::setRAAN>());
    cls->addProperty("LAN",             aNewPropertyDouble<StateKeplerian, &StateKeplerian::getLAN, &StateKeplerian::setLAN>());

    cls->addProperty("ArgPeri",         aNewPropertyDouble<StateKeplerian, &StateKeplerian::getArgPeri, &StateKeplerian::setArgPeri>());
    
    cls->addProperty("TrueAnomaly",     aNewPropertyDouble<StateKeplerian, &StateKeplerian::getTrueAnomaly, &StateKeplerian::setTrueAnomaly>());
    cls->addProperty("MeanAnomaly",     aNewPropertyDouble<StateKeplerian, &StateKeplerian::getMeanAnomaly, &StateKeplerian::setMeanAnomaly>());
    cls->addProperty("EccAnomaly",      aNewPropertyDouble<StateKeplerian, &StateKeplerian::getEccAnomaly, &StateKeplerian::setEccAnomaly>());
    cls->addProperty("ArgLat",          aNewPropertyDouble<StateKeplerian, &StateKeplerian::getArgLat, &StateKeplerian::setArgLat>());
    cls->addProperty("TimePastPeri",    aNewPropertyDouble<StateKeplerian, &StateKeplerian::getTimePastPeri, &StateKeplerian::setTimePastPeri>());
    cls->addProperty("TimePastAscNode", aNewPropertyDouble<StateKeplerian, &StateKeplerian::getTimePastAscNode, &StateKeplerian::setTimePastAscNode>());
}

AST_NAMESPACE_END
