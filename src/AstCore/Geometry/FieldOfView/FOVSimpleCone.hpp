///
/// @file      FOVSimpleCone.hpp
/// @brief     简单圆锥视场类
/// @details   实现简单圆锥视场的属性和方法
/// @author    axel
/// @date      2026-04-09
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

#pragma once

#include "FieldOfView.hpp"

AST_NAMESPACE_BEGIN

/// @brief 简单圆锥视场类
class AST_CORE_API FOVSimpleCone: public FieldOfView
{
public:
    /// @brief 构造函数
    FOVSimpleCone() = default;
    
    /// @brief 析构函数
    virtual ~FOVSimpleCone() = default;
    
    /// @brief 获取视场类型
    /// @return 视场类型
    virtual EFOVType getFOVType() const override { return EFOVType::eSimpleCone; }
    
    /// @brief 设置锥角
    /// @param angle 锥角（度）
    void setConeAngle(double angle) { coneAngle_ = angle; }
    
    /// @brief 获取锥角
    /// @return 锥角（度）
    double getConeAngle() const { return coneAngle_; }
    
    /// @brief 设置角分辨率
    /// @param resolution 角分辨率（度）
    void setAngularPatternResolution(double resolution) { angularPatternResolution_ = resolution; }
    
    /// @brief 获取角分辨率
    /// @return 角分辨率（度）
    double getAngularPatternResolution() const { return angularPatternResolution_; }
    
private:
    double coneAngle_{0.0};                ///< 锥角（度）
    double angularPatternResolution_{0.0};  ///< 角分辨率（度）
};

AST_NAMESPACE_END