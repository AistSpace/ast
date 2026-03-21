///
/// @file      EphemerisLagrangeVar.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstSim/Ephemeris.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/Frame.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 可变步长拉格朗日插值星历
class AST_SIM_API EphemerisLagrangeVar: public Ephemeris
{
public:
    EphemerisLagrangeVar() = default;
    ~EphemerisLagrangeVar() = default;
public:
    Frame* getFrame() const override;
    err_t getPos(const TimePoint& tp, Vector3d& pos) const override;
    err_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override;
protected:
    SharedPtr<Frame>      frame_;                   ///< 参考坐标系
    std::vector<double>   times_;                   ///< 时间(单位：秒)
    std::vector<Vector3d> positions_;               ///< 位置(单位：米)
    std::vector<Vector3d> velocities_;              ///< 速度(单位：米/秒)
    TimePoint             epoch_{};                 ///< 历元时间
    double                step_{60};                ///< 时间步长
    int                   interpolateOrder_{1};     ///< 插值阶数
};


/*! @} */

AST_NAMESPACE_END
