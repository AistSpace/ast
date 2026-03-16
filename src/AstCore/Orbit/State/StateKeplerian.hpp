///
/// @file      StateKeplerian.hpp
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
#include "AstCore/State.hpp"
#include "AstCore/OrbitElement.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/
class StateKeplerian;
using PStateKeplerian = StateKeplerian*;
using HStateKeplerian = SharedPtr<StateKeplerian>;


/// @brief 轨道大小描述方式
enum class ESizeType
{
    eSMA,                   ///< 半长轴
    eApoAlt,                ///< 远地点高度
    eApoRad,                ///< 远地点半径
    ePeriAlt,               ///< 近地点高度
    ePeriRad,               ///< 近地点半径
    ePeriod,                ///< 周期
    eMeanMotion,            ///< 平均角速度
};

/// @brief 轨道形状描述方式
enum class EShapeType
{
    eEcc,                   ///< 偏心率
    eApoAlt,                ///< 远地点高度
    eApoRad,                ///< 远地点半径
    ePeriAlt,               ///< 近地点高度
    ePeriRad,               ///< 近地点半径
};

/// @brief 轨道面方向描述方式
enum class EOrientationType
{
    eRAAN,                  ///< 升交点赤经
    eLAN,                   ///< 升交点经度
};

/// @brief 轨道位置描述方式
enum class EPositionType
{
    eTrueAnomaly,               ///< 真近点角
    eMeanAnomaly,               ///< 平近点角
    eEccAnomaly,                ///< 偏近点角   
    eArgLat,                    ///< 纬度幅角
    eTimePastPeri,              ///< 过近地点后经过的时间
    eTimePastAscNode,           ///< 过升交点后经过的时间
    eTimeOfPeriPassage,         ///< 过近地点时刻
    eTimeOfAscNodePassage,      ///< 过升交点时刻
};




/// @brief 开普勒轨道根数状态
class AST_CORE_API StateKeplerian final: public State
{
public:
    static PStateKeplerian New();
    static HStateKeplerian MakeShared();

    StateKeplerian() = default;
    ~StateKeplerian() override = default;
public:
    err_t getState(CartState& state) const override;
    err_t setState(const CartState& state) override;
public:
    void setState(const ModOrbElem& modOrbElem){ modOrbElem_ = modOrbElem; }
PROPERTIES:
    /// @brief 获取半长轴
    double getSMA() const;
    /// @brief 设置半长轴
    void setSMA(double sma);

    /// @brief 获取平均角速度
    double getMeanMotion() const;
    /// @brief 设置平均角速度
    void setMeanMotion(double meanMotion);

    /// @brief 获取周期
    double getPeriod() const;
    /// @brief 设置周期
    void setPeriod(double period);

    /// @brief 获取远地点高度
    double getApoAlt() const;
    /// @brief 设置远地点高度（描述轨道大小）
    void setApoAltForSize(double apoAlt);
    /// @brief 设置远地点高度（描述轨道形状）
    void setApoAltForShape(double apoAlt);

    /// @brief 获取远地点半径
    double getApoRad() const;
    /// @brief 设置远地点半径（描述轨道大小）
    void setApoRadForSize(double apoRad);
    /// @brief 设置远地点半径（描述轨道形状）
    void setApoRadForShape(double apoRad);

    /// @brief 获取近地点高度
    double getPeriAlt() const;
    /// @brief 设置近地点高度（描述轨道大小）
    void setPeriAltForSize(double periAlt);
    /// @brief 设置近地点高度（描述轨道形状）
    void setPeriAltForShape(double periAlt);

    /// @brief 获取近地点半径
    double getPeriRad() const;
    /// @brief 设置近地点半径（描述轨道大小）
    void setPeriRadForSize(double periRad);
    /// @brief 设置近地点半径（描述轨道形状）
    void setPeriRadForShape(double periRad);
    
    /// @brief 获取离心率
    double getEcc() const;
    /// @brief 设置离心率
    void setEcc(double ecc);

    /// @brief 获取倾角
    double getInc() const;
    /// @brief 设置倾角
    void setInc(double inc);

    /// @brief 获取升交点赤经   
    double getRAAN() const;
    /// @brief 设置升交点赤经
    void setRAAN(double raan);

    /// @brief 获取升交点经度   
    double getLAN() const;
    /// @brief 设置升交点经度
    void setLAN(double lan);

    /// @brief 获取近地点幅角  
    double getArgPeri() const;
    /// @brief 设置近地点幅角
    void setArgPeri(double argPeri);

    /// @brief 获取真近点角
    double getTrueAnomaly() const;
    /// @brief 设置真近点角
    void setTrueAnomaly(double trueAnomaly);

    /// @brief 获取平近点角
    double getMeanAnomaly() const;
    /// @brief 设置平近点角
    void setMeanAnomaly(double meanAnomaly);

    /// @brief 获取偏近点角
    double getEccAnomaly() const;
    /// @brief 设置偏近点角
    void setEccAnomaly(double eccAnomaly);

    /// @brief 获取纬度幅角  
    double getArgLat() const;
    /// @brief 设置纬度幅角
    void setArgLat(double argLat);

    /// @brief 获取过近地点后经过的时间
    double getTimePastPeri() const;
    /// @brief 设置过近地点后经过的时间
    void setTimePastPeri(double timePastPeri);

    /// @brief 获取过升交点后经过的时间
    double getTimePastAscNode() const;
    /// @brief 设置过升交点后经过的时间
    void setTimePastAscNode(double timePastAscNode);

    /// @brief 获取过近地点时刻
    void getTimeOfPeriPassage(TimePoint& tp) const;
    /// @brief 设置过近地点时刻
    void setTimeOfPeriPassage(const TimePoint& tp);

    /// @brief 获取过升交点时刻
    void getTimeOfAscNodePassage(TimePoint& tp) const;
    /// @brief 设置过升交点时刻
    void setTimeOfAscNodePassage(const TimePoint& tp);

protected:
    ModOrbElem modOrbElem_{};                                       ///< 轨道根数
    ESizeType  sizeType_{ESizeType::eSMA};                          ///< 大小描述方式
    EShapeType shapeType_{EShapeType::eEcc};                        ///< 形状描述方式
    EOrientationType orientationType_{EOrientationType::eRAAN};     ///< 轨道面方向描述方式
    EPositionType positionType_{EPositionType::eTrueAnomaly};       ///< 位置描述方式
};

/*! @} */

AST_NAMESPACE_END
