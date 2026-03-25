///
/// @file      MotionHPOP.cpp
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

#include "MotionHPOP.hpp"
#include "AstSim/MotionProfileVisitor.hpp"
#include "AstCore/HPOP.hpp"

AST_NAMESPACE_BEGIN

PMotionHPOP MotionHPOP::New()
{
    return new MotionHPOP();
}

err_t MotionHPOP::makeEphemerisSpec(ScopedPtr<Ephemeris> &eph) const
{
    return makeEphemerisSimple(eph);
}

err_t MotionHPOP::makeEphemerisSimple(ScopedPtr<Ephemeris> &eph) const
{
    err_t rc;
    PropagationParams params;
    rc = this->getPropagationParams(params);   AST_CHECK_ERRCODE(rc, "failed to get propagation params");
    auto propFrame = params.propagationFrame_;
    const TimePoint& epoch = params.epoch_;
    const CartState& cartState = params.stateInPropagationFrame_;

    std::vector<double> times;
    std::vector<Vector3d> positions, velocities;
    
    HPOP hpop;
    hpop.setForceModel(this->forceModel_);
    A_UNUSED(propFrame);
    A_UNUSED(epoch);
    A_UNUSED(cartState);

    return -1;
}

void MotionHPOP::accept(MotionProfileVisitor& visitor)
{
    visitor.visit(*this);
}

AST_NAMESPACE_END