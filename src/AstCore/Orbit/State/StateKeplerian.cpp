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

AST_NAMESPACE_BEGIN

PStateKeplerian StateKeplerian::New()
{
    return new StateKeplerian();
}

HStateKeplerian StateKeplerian::MakeShared()
{
    return new StateKeplerian();
}

err_t StateKeplerian::getState(CartState &state) const
{
    return aModOrbElemToCart(modOrbElem_, getGM(), state.pos(), state.vel());
}

err_t StateKeplerian::setState(const CartState &state)
{
    return aCartToModOrbElem(state.pos(), state.vel(), getGM(), modOrbElem_);
}

double StateKeplerian::getMeanMotion() const
{
    return modOrbElem_.getMeanMotion(getGM());
}

double StateKeplerian::getPeriod() const
{
    return modOrbElem_.getPeriod(getGM());
}

double StateKeplerian::getApoAlt() const
{
    return modOrbElem_.getApoAlt(getBodyRadius());
}

double StateKeplerian::getApoRad() const
{
    return modOrbElem_.getApoRad();
}

double StateKeplerian::getPeriAlt() const
{
    return modOrbElem_.getPeriAlt(getBodyRadius());
}

double StateKeplerian::getPeriRad() const
{
    return modOrbElem_.getPeriRad();
}

double StateKeplerian::getEcc() const
{
    return modOrbElem_.getEcc();
}

double StateKeplerian::getInc() const
{
    return modOrbElem_.getInc();
}

double StateKeplerian::getRAAN() const
{
    return modOrbElem_.getRAAN();
}


double StateKeplerian::getSMA() const
{
    return modOrbElem_.getSMA();
}

double StateKeplerian::getArgPeri() const
{
    return modOrbElem_.getArgPeri();
}

double StateKeplerian::getTrueAnomaly() const
{
    return modOrbElem_.getTrueAnomaly();
}

double StateKeplerian::getMeanAnomaly() const
{
    return modOrbElem_.getMeanAnomaly();
}

double StateKeplerian::getEccAnomaly() const
{
    return modOrbElem_.getEccAnomaly();
}

double StateKeplerian::getArgLat() const
{
    return modOrbElem_.getArgLat();
}

double StateKeplerian::getTimePastPeri() const
{
    return modOrbElem_.getTimePastPeri(getGM());
}

double StateKeplerian::getTimePastAscNode() const
{
    return modOrbElem_.getTimePastAscNode(getGM());
}

AST_NAMESPACE_END


