///
/// @file      EventDetector.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-27
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
#include "AstMath/ODEEventDetector.hpp"

AST_NAMESPACE_BEGIN


class SpacecraftState;


/// @brief 事件检测基类
/// 事件检测基类，用于检测事件是否发生。
/// 参考orekit的EventDetector类
class AST_CORE_API EventDetector
{
public:
    using EDirection = ODEEventDetector::EDirection;


    EventDetector() = default;
    virtual ~EventDetector() = default;

    /// @brief 获取事件检测开关函数与目标值的差值
    /// @param state 航天器状态
    /// @param t 时间（相对于预报器参考时间的秒数）
    /// @return 事件检测开关函数与目标值的差值
    virtual double getDifference(const SpacecraftState& state, double t) const{return getValue(state, t) - goal_;}

    /// @brief 获取事件检测开关函数的值
    /// @param state 航天器状态
    /// @param t 时间（相对于预报器参考时间的秒数）
    /// @return 事件检测开关函数的值
    virtual double getValue(const SpacecraftState& state, double t) const = 0;
private:
    int         repeatCount_{1};                ///< 事件触发后的重复次数
    EDirection  direction_{EDirection::eBoth};  ///< 事件检测开关函数的方向
    double      threshold_{1e-10};              ///< 事件检测开关函数的阈值
    double      goal_{0.0};                     ///< 事件检测的目标值
};


/// @brief 泛型事件检测器
/// @details 
template<typename Func>
class EventDetectorGeneric: public EventDetector
{
public:
    explicit EventDetectorGeneric(Func func) 
        : func_(std::move(func)) 
    {}
    
    double getValue(const SpacecraftState& state, double t) const override {
        return func_(state, t);
    }
    
private:
    Func func_;
};


AST_NAMESPACE_END
