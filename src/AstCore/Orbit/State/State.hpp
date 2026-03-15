///
/// @file      State.hpp
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
#include "AstUtil/Object.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/EventTime.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 航天器状态
/// @details 参考了orekit的Orbit类
/// 但是这个可能不只是限制于轨道状态，所以取名为State
class AST_CORE_API State: public Object
{
public:
    State() = default;
    ~State() override = default;
public:

public:
    /// @brief 获取参考坐标系
    Frame* getFrame() const{ return frame_.get(); }

    /// @brief 设置参考坐标系
    void setFrame(Frame* frame){ frame_ = frame; }

    /// @brief 改变参考坐标系
    /// @param frame 新的参考坐标系
    /// @return err_t 错误码
    err_t changeFrame(Frame* frame);

    /// @brief 设置状态历元时间
    /// @param stateEpoch 状态历元时间
    void setStateEpoch(EventTime* stateEpoch);

    /// @brief 设置状态历元时间
    /// @param stateEpoch 状态历元时间
    void setStateEpoch(const TimePoint& stateEpoch);

    /// @brief 获取状态历元时间
    /// @param stateEpoch 状态历元时间
    /// @return err_t 错误码
    err_t getStateEpoch(TimePoint& stateEpoch) const;

    /// @brief 获取状态历元时间句柄
    /// @return SharedPtr<EventTime>& 状态历元时间句柄
    SharedPtr<EventTime>& getStateEpochHandle(){ return stateEpoch_; }

    /// @brief 获取引力常数
    /// @return double 引力常数
    double getGM() const { return gm_; }

    /// @brief 设置引力常数
    /// @param gm 引力常数
    void setGM(double gm){ gm_ = gm; }

public:
    void setCoordEpoch(EventTime* coordEpoch);

    void setCoordEpoch(const TimePoint& coordEpoch);

    err_t getCoordEpoch(TimePoint& coordEpoch) const;

    bool  getUseCoordEpoch() const;

    void  setUseCoordEpoch(bool useCoordEpoch);
protected:
    SharedPtr<Frame>        frame_;                ///< 参考坐标系
    double                  gm_{0};                ///< 引力常数
    SharedPtr<EventTime>    stateEpoch_;           ///< 状态历元时间
};

/*! @} */

AST_NAMESPACE_END
