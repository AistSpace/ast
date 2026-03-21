///
/// @file      EphemerisLagrangeVar.cpp
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

#include "EphemerisLagrangeVar.hpp"
#include "AstMath/LagrangeInterpolator.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

Frame *EphemerisLagrangeVar::getFrame() const
{
    return frame_.get();
}

err_t EphemerisLagrangeVar::getPos(const TimePoint &tp, Vector3d &pos) const
{
    double delta = tp - epoch_;
    double index_d = delta / step_;
    int index = static_cast<int>(index_d);
    if(index < 0 || index >= static_cast<int>(positions_.size())){
        return eErrorOutOfRange;
    }
    // 刚好在时间点上
    if(index_d == index){
        pos = positions_[index];
        return 0;
    }else{
        // 插值
        int n = std::min(interpolateOrder_ + 1, static_cast<int>(positions_.size()));
        int start = index - n / 2;
        if(start < 0){
            start = 0;
        }
        // int end = start + n;
        aLagrangeInterpolate(times_.data() + start, positions_[start].data(), n, 3, delta, pos.data());
    }
    return eNoError;
}

err_t EphemerisLagrangeVar::getPosVel(const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    double delta = tp - epoch_;
    double index_d = delta / step_;
    int index = static_cast<int>(index_d);
    if(index < 0 || index >= static_cast<int>(positions_.size())){
        return eErrorOutOfRange;
    }
    // 刚好在时间点上
    if(index_d == index){
        pos = positions_[index];
        vel = velocities_[index];
        return 0;
    }else{
        // 插值
        int n = std::max(interpolateOrder_ + 1, static_cast<int>(positions_.size()));
        int start = index - n / 2;
        if(start < 0){
            start = 0;
        }
        // int end = start + n;
        aLagrangeInterpolate(times_.data() + start, positions_[start].data(), n, 3, delta, pos.data());
        aLagrangeInterpolate(times_.data() + start, velocities_[start].data(), n, 3, delta, vel.data());
    }
    return eNoError;
}

AST_NAMESPACE_END

