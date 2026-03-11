///
/// @file      FrameAssembly.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-11
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

#include "FrameAssembly.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Transform.hpp"
#include "AstMath/KinematicTransform.hpp"


AST_NAMESPACE_BEGIN

FrameAssembly::FrameAssembly(Axes *axes, Point *origin)
    : Frame()
    , axes_(axes)
    , origin_(origin)
{
}


Frame *FrameAssembly::getParent() const
{
    if(origin_)
        return origin_->getFrame();
    return nullptr;
}

err_t FrameAssembly::getTransform(const TimePoint &tp, Transform &transform) const
{
    auto parent = getParent();
    if(!parent)
        return eErrorInvalidParam;
    if(origin_)
    {
        err_t ret = origin_->getPosIn(parent, tp, transform.getTranslation());
        if(ret != 0)
            return ret;
    }
    if(axes_)
    {
        err_t ret = axes_->getTransformFrom(parent->getAxes(), tp, transform.getRotation());
        if(ret != 0)
            return ret;
    }
    return eNoError;
}

err_t FrameAssembly::getTransform(const TimePoint &tp, KinematicTransform &transform) const
{
    auto parent = getParent();
    if(!parent)
        return eErrorInvalidParam;
    if(origin_)
    {
        err_t ret = origin_->getPosVelIn(parent, tp, transform.getTranslation(), transform.getVelocity());
        if(ret != 0)
            return ret;
    }
    if(axes_)
    {
        err_t ret = axes_->getTransformFrom(parent->getAxes(), tp, transform.getKinematicRotation());
        if(ret != 0)
            return ret;
    }
    return eNoError;
}

AST_NAMESPACE_END


