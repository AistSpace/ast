///
/// @file      EphemerisTwoBody.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
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

#include "EphemerisTwoBody.hpp"
#include "AstCore/TwoBody.hpp"
#include "AstCore/BuiltinFrame.hpp"

AST_NAMESPACE_BEGIN

EphemerisTwoBody::EphemerisTwoBody()
    : frame_{aFrameECI()}
{

}

EphemerisTwoBody::EphemerisTwoBody(Frame *frame, double gm, const TimePoint &epoch, const Vector3d &pos, const Vector3d &vel)
    : frame_{frame}
    , gm_{gm}
    , epoch_{epoch}
    , initpos_{pos}
    , initvel_{vel}
{
}

Frame *EphemerisTwoBody::getFrame() const
{
    return frame_.get();
}

err_t EphemerisTwoBody::getPos(const TimePoint &tp, Vector3d &pos) const
{
    double duration = tp.durationFrom(epoch_);
    pos = initpos_;
    Vector3d vel = initvel_;
    return aTwoBodyProp(duration, gm_, pos, vel);
}

err_t EphemerisTwoBody::getPosVel(const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    double duration = tp.durationFrom(epoch_);
    pos = initpos_;
    vel = initvel_;
    return aTwoBodyProp(duration, gm_, pos, vel);
}

AST_NAMESPACE_END

