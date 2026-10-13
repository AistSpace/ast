///
/// @file      BlendingAtmosphere.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-13
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "BlendingAtmosphere.hpp"
#include <cassert>

AST_NAMESPACE_BEGIN

BlendingAtmosphere::BlendingAtmosphere(Atmosphere* mainModel, Atmosphere* lowAltModel, double blendAltitude, double blendRange)
    : Atmosphere()
    , mainModel_(mainModel)
    , lowAltModel_(lowAltModel)
    , blendAltitude_(blendAltitude)
    , blendRange_(blendRange)
{
    assert(blendAltitude_ >= 0.0);
    assert(blendRange_ >= 0.0);
    assert(mainModel_ != nullptr);
    assert(lowAltModel_ != nullptr);
}

BlendingAtmosphere::BlendingAtmosphere(Atmosphere* mainModel, Atmosphere* lowAltModel)
    : Atmosphere()
    , mainModel_(mainModel)
    , lowAltModel_(lowAltModel)
{
    assert(mainModel_ != nullptr);
    assert(lowAltModel_ != nullptr);
}

double BlendingAtmosphere::getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const
{
    double altitude = getAltitude(posInBodyFixed);  // [m]

    // 无过渡带：直接按中心高度二选一
    if(blendRange_ <= 0.0)
    {
        return altitude < blendAltitude_ ? lowAltModel_->getDensity(tp, posInBodyFixed) : mainModel_->getDensity(tp, posInBodyFixed);
    }

    double halfRange = 0.5 * blendRange_;
    double lowerAlt  = blendAltitude_ - halfRange;
    double upperAlt  = blendAltitude_ + halfRange;

    // 过渡带以下：全部使用低高度模型
    if(altitude <= lowerAlt)
    {
        return lowAltModel_->getDensity(tp, posInBodyFixed);
    }
    // 过渡带以上：全部使用主模型
    if(altitude >= upperAlt)
    {
        return mainModel_->getDensity(tp, posInBodyFixed);
    }
    // 过渡带内：按高度对两者线性加权（低位偏重低高度模型，高位偏重主模型）
    double weight = (altitude - lowerAlt) / blendRange_;  // 0 → 1
    return weight * mainModel_->getDensity(tp, posInBodyFixed) + (1.0 - weight) * lowAltModel_->getDensity(tp, posInBodyFixed);
}

Frame* BlendingAtmosphere::getFrame() const
{
    return mainModel_->getFrame();
}

BodyShape* BlendingAtmosphere::getShape() const
{
    return mainModel_->getShape();
}


AST_NAMESPACE_END
