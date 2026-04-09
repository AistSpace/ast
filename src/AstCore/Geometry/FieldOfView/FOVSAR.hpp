///
/// @file      FOVSAR.hpp
/// @brief     合成孔径雷达视场类
/// @details   实现合成孔径雷达视场的属性和方法
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

/// @brief 合成孔径雷达视场类
class AST_CORE_API FOVSAR: public FieldOfView
{
public:
    /// @brief 构造函数
    FOVSAR() = default;
    
    /// @brief 析构函数
    virtual ~FOVSAR() = default;
    
    /// @brief 获取视场类型
    /// @return 视场类型
    virtual EFOVType getFOVType() const override { return EFOVType::eSAR; }
    
    /// @brief 设置方位半角
    /// @param angle 方位半角（度）
    void setAzimuthHalfAngle(double angle) { azimuthHalfAngle_ = angle; }
    
    /// @brief 获取方位半角
    /// @return 方位半角（度）
    double getAzimuthHalfAngle() const { return azimuthHalfAngle_; }
    
    /// @brief 设置距离半角
    /// @param angle 距离半角（度）
    void setRangeHalfAngle(double angle) { rangeHalfAngle_ = angle; }
    
    /// @brief 获取距离半角
    /// @return 距离半角（度）
    double getRangeHalfAngle() const { return rangeHalfAngle_; }
    
private:
    double azimuthHalfAngle_{0.0};  ///< 方位半角（度）
    double rangeHalfAngle_{0.0};    ///< 距离半角（度）
};

AST_NAMESPACE_END