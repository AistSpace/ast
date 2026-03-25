///
/// @file      SpacecraftState.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-25
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "AstCore/State.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 空间飞行器状态量
class SpacecraftState
{
public:
    SpacecraftState() = default;
    ~SpacecraftState() = default;
public:
    /// @brief 获取质量
    /// @return 质量
    double getMass() const{return mass_;}

    /// @brief 设置质量
    /// @param mass 质量
    void setMass(double mass){mass_ = mass;}

    /// @brief 获取轨道状态
    /// @return 轨道状态
    const State* getOrbitState() const{return orbitState_;}

    /// @brief 设置轨道状态
    /// @param orbitState 轨道状态
    void setOrbitState(State* orbitState){orbitState_ = orbitState;}
protected:
    HState orbitState_;     ///< 轨道状态
    double mass_;           ///< 质量
};

/*! @} */

AST_NAMESPACE_END
