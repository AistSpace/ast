///
/// @file      Point.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-09
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

#include "Point.hpp"
#include "AstCore/Frame.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Transform.hpp"
#include "AstMath/KinematicTransform.hpp"

AST_NAMESPACE_BEGIN

err_t Point::getPosIn(Frame *frame, const TimePoint &tp, Vector3d &pos) const
{
    auto parent = frame->getParent();
    if(parent == frame)
    {
        return getPos(tp, pos);
    }
    else
    {
        Vector3d posInParent;
        err_t rc = getPos(tp, posInParent);
        if(rc) return rc;
        Transform transform;
        rc = aFrameTransform(parent, frame, tp, transform);
        if(rc) return rc;
        transform.transformPosition(posInParent, pos);
        return eNoError;
    }
}

err_t Point::getPosVelIn(Frame *frame, const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    auto parent = frame->getParent();
    if(parent == frame)
    {
        return getPosVel(tp, pos, vel);
    }
    else
    {
        Vector3d posInParent, velInParent;
        err_t rc = getPosVel(tp, posInParent, velInParent);
        if(rc) return rc;
        KinematicTransform transform;
        rc = aFrameTransform(parent, frame, tp, transform);
        if(rc) return rc;
        transform.transformPositionVelocity(posInParent, velInParent, pos, vel);
        return eNoError;
    }
}

AST_NAMESPACE_END


