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
#include "AstUtil/Dimension.hpp"

AST_NAMESPACE_BEGIN

void StateKeplerian_ClassInit(Class* cls)
{
    cls->addProperty("SMA",             aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getSMA, &StateKeplerian::setSMA>(Dimension::Length()));
    cls->addProperty("Period",          aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getPeriod, &StateKeplerian::setPeriod>(Dimension::Time()));
    cls->addProperty("MeanMotion",      aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getMeanMotion, &StateKeplerian::setMeanMotion>(Dimension::Frequency()));
    cls->addProperty("ApoRadForSize",   aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getApoRad, &StateKeplerian::setApoRadForSize>(Dimension::Length()));
    cls->addProperty("ApoAltForSize",   aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getApoAlt, &StateKeplerian::setApoAltForSize>(Dimension::Length()));
    cls->addProperty("PeriRadForSize",  aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getPeriRad, &StateKeplerian::setPeriRadForSize>(Dimension::Length()));
    cls->addProperty("PeriAltForSize",  aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getPeriAlt, &StateKeplerian::setPeriAltForSize>(Dimension::Length()));

    cls->addProperty("Ecc",             aNewPropertyDouble<StateKeplerian, &StateKeplerian::getEcc, &StateKeplerian::setEcc>());
    cls->addProperty("ApoRadForShape",  aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getApoRad, &StateKeplerian::setApoRadForShape>(Dimension::Length()));
    cls->addProperty("ApoAltForShape",  aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getApoAlt, &StateKeplerian::setApoAltForShape>(Dimension::Length()));
    cls->addProperty("PeriRadForShape", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getPeriRad, &StateKeplerian::setPeriRadForShape>(Dimension::Length()));
    cls->addProperty("PeriAltForShape", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getPeriAlt, &StateKeplerian::setPeriAltForShape>(Dimension::Length()));

    cls->addProperty("Inc",             aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getInc, &StateKeplerian::setInc>(Dimension::Angle()));

    cls->addProperty("RAAN",            aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getRAAN, &StateKeplerian::setRAAN>(Dimension::Angle()));
    cls->addProperty("LAN",             aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getLAN, &StateKeplerian::setLAN>(Dimension::Angle()));

    cls->addProperty("ArgPeri",         aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getArgPeri, &StateKeplerian::setArgPeri>(Dimension::Angle()));
    
    cls->addProperty("TrueAnomaly",     aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getTrueAnomaly, &StateKeplerian::setTrueAnomaly>(Dimension::Angle()));
    cls->addProperty("MeanAnomaly",     aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getMeanAnomaly, &StateKeplerian::setMeanAnomaly>(Dimension::Angle()));
    cls->addProperty("EccAnomaly",      aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getEccAnomaly, &StateKeplerian::setEccAnomaly>(Dimension::Angle()));
    cls->addProperty("ArgLat",          aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getArgLat, &StateKeplerian::setArgLat>(Dimension::Angle()));
    cls->addProperty("TimePastPeri",    aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getTimePastPeri, &StateKeplerian::setTimePastPeri>(Dimension::Time()));
    cls->addProperty("TimePastAscNode", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getTimePastAscNode, &StateKeplerian::setTimePastAscNode>(Dimension::Time()));
}

AST_NAMESPACE_END
