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
    double getMass() const{return fuelMass_ + dryMass_;}
    double getFuelMass() const{return fuelMass_;}
    double getDryMass() const{return dryMass_;}

    /// @brief 设置质量
    /// @param mass 质量
    void setFuelMass(double fuelMass){fuelMass_ = fuelMass;}
    void setDryMass(double dryMass){dryMass_ = dryMass;}

    double getCd() const{return cd_;}
    void setCd(double cd){cd_ = cd;}

    double getCr() const{return cr_;}
    void setCr(double cr){cr_ = cr;}
    
    double getDragArea() const{return dragArea_;}
    void setDragArea(double dragArea){dragArea_ = dragArea;}

    double getSRPArea() const{return srpArea_;}
    void setSRPArea(double srpArea){srpArea_ = srpArea;}

    double getK1() const{return k1_;}
    void setK1(double k1){k1_ = k1;}

    double getK2() const{return k2_;}
    void setK2(double k2){k2_ = k2;}

    double getFuelDensity() const{return fuelDensity_;}
    void setFuelDensity(double fuelDensity){fuelDensity_ = fuelDensity;}

    double getRadPressureArea() const{return radPressureArea_;}
    void setRadPressureArea(double radPressureArea){radPressureArea_ = radPressureArea;}

    double getRadPressureCoeff() const{return radPressureCoeff_;}
    void setRadPressureCoeff(double radPressureCoeff){radPressureCoeff_ = radPressureCoeff;}


    double getTankPressure() const{return tankPressure_;}
    void setTankPressure(double tankPressure){tankPressure_ = tankPressure;}

    double getTankTemperature() const{return tankTemperature_;}
    void setTankTemperature(double tankTemperature){tankTemperature_ = tankTemperature;}


    /// @brief 获取轨道状态
    /// @return 轨道状态
    const State* getOrbitState() const{return orbitState_;}

    /// @brief 设置轨道状态
    /// @param orbitState 轨道状态
    void setOrbitState(State* orbitState){orbitState_ = orbitState;}
protected:
    HState orbitState_;                 ///< 轨道状态
    double cd_{2.2};
    double cr_{1};                     
    double dragArea_{20};
    double srpArea_{20};
    double dryMass_{500};
    double fuelMass_{500};
    double fuelDensity_{1000};
    double k1_{1};
    double k2_{1};
    double radPressureArea_{20};
    double radPressureCoeff_{1};
    double tankPressure_{5000};
    double tankTemperature_{293.15};
};

/*! @} */

AST_NAMESPACE_END
