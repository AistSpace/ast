///
/// @file      MotionBasic.hpp
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
#include "AstSim/MotionProfile.hpp"
#include "AstCore/State.hpp"
#include "AstCore/EventInterval.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 基础运动模型数据结构
class AST_SIM_API MotionBasic: public MotionProfile
{
public:
    MotionBasic() = default;
    ~MotionBasic() = default;

    /// @brief 获取初始状态
    /// @return 状态指针
    State* getInitialState() const { return initialState_.get(); }

    /// @brief 获取运动时间间隔
    /// @param interval 时间间隔引用
    /// @return 错误码
    err_t getInterval(TimeInterval& interval) const;

    /// @brief 获取预报坐标系
    /// @return 预报坐标系指针
    Frame* getPropagationFrame() const { return propagationFrame_.get(); }

    /// @brief 获取预报步长
    /// @return 预报步长
    double getStepSize() const { return stepSize_; }
protected:
    SharedPtr<State>            initialState_;          ///< 初始状态
    SharedPtr<EventInterval>    interval_;              ///< 时间段
    SharedPtr<Frame>            propagationFrame_;      ///< 预报坐标系
    double                      stepSize_;              ///< 生成星历步长
};

/*! @} */

AST_NAMESPACE_END
