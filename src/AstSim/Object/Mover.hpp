///
/// @file      Mover.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/Object.hpp"
#include "AstCore/Point.hpp"
#include "AstSim/AttitudeProfile.hpp"
#include "AstSim/MotionProfile.hpp"
#include "AstSim/Ephemeris.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 运动对象
class Mover: public Point
{
public:
    Mover() = default;
    ~Mover() override = default;

    /// @brief 获取运动定义
    /// @return 运动定义指针
    MotionProfile* getMotionProfile() const { return motionProfile_.get(); }
    
    /// @brief 获取姿态定义
    /// @return 姿态定义指针
    AttitudeProfile* getAttitudeProfile() const { return attitudeProfile_.get(); }
    
    /// @brief 获取星历
    /// @return 星历指针
    Ephemeris* getEphemeris() const { return ephemeris_.get(); }

public: // 从Point继承重写的函数
    Frame* getFrame() const final;
    err_t getPos(const TimePoint& tp, Vector3d& pos) const final;
    err_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const final;
protected:
    ScopedPtr<MotionProfile>    motionProfile_;         ///< 运动定义
    ScopedPtr<AttitudeProfile>  attitudeProfile_;       ///< 姿态定义
    ScopedPtr<Ephemeris>        ephemeris_;             ///< 星历
};

/*! @} */

AST_NAMESPACE_END
