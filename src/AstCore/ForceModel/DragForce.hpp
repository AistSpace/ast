///
/// @file      DragForce.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-21
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

#include "AstGlobal.h"
#include "ForceModel.hpp"
#include "AstUtil/EnumOrName.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @ingroup Core
    @defgroup ForceModel
    @{
*/


enum class EAtmosphereModel
{
    eNone,
    e1976Standard,
    eHarrisPriester,
    eJacchia60,
    eJacchia70,
    eJacchia71,
    eJacchiaRoberts,
    eMSIS1986,
    eMSISE1990,
    eNRLMSISE2000,
    eCIRA72,
    eDTM2012,
};

using EAtmDensityModel = EAtmosphereModel;   // alias

/// @brief 大气模型标识：既可赋内置枚举模型，也可赋模型的名字
/// @code
///   drag.atmDensityModel_ = EAtmosphereModel::eMSISE1990;   // 内置模型枚举
///   drag.atmDensityModel_ = "Exponential";                  // 内置模型名称，或者插件模型名称
/// @endcode
using AtmosphereModelId = EnumOrName<EAtmosphereModel>;

enum class EGeoMagFluxSource
{
    eKp,
    eAp,
};

enum class EGeoMagFluxUpdateRate
{
    eDaily,
    e3Hourly,
    e3HourlyInterp,
    e3HourlyCubicSpline,
};

/// @brief 大气阻力模型
class AST_CORE_API DragForce : public ForceModel
{
public:
    AST_OBJECT(DragForce)
    DragForce() = default;
    ~DragForce() = default;

    /// @brief 地磁活动 ap 指数
    double ap() const;

    /// @brief 地磁活动 Kp 指数
    double kp() const;

    /// @brief 当日 F10.7 太阳射电流量
    double f10p7Daily() const;

    /// @brief 平均 F10.7 太阳射电流量
    double f10p7Average() const;

    /// @brief 大气模型
    AtmosphereModelId& atmosphereModel() { return atmDensityModel_; }

    /// @brief 低高度大气模型
    AtmosphereModelId& lowAltAtmDensityModel() { return lowAltAtmDensityModel_; }

public:
    /// @name 大气密度模型配置
    /// @{
    AtmosphereModelId atmDensityModel_{EAtmosphereModel::e1976Standard};        ///< 大气模型（内置枚举、模型名称或插件模型名称）
    AtmosphereModelId lowAltAtmDensityModel_{EAtmosphereModel::eNone};          ///< 低高度大气模型（通常为 MSISE1990 / NRLMSISE2000）
    double atmBlendingRange_{0.0};                                              ///< 主/低高度模型混合过渡范围 [m]
    /// @}

    
    /// @name 空间天气数据源
    /// @{
    bool useFluxApFile_{false};                                                 ///< 是否从外部文件读取地磁/太阳通量数据

    /// @name 手动输入的太阳/地磁指数（useFluxApFile_ == false 时生效）
    /// @{
    double f10p7Average_{150.0};                                                ///< 平均 F10.7 太阳射电流量
    double f10p7Daily_{150.0};                                                  ///< 当日 F10.7 太阳射电流量
    double kp_{3.0};                                                            ///< 地磁活动 Kp 指数
    /// @}

    /// @name 文件数据配置参数（useFluxApFile_ == true 时生效）
    /// @{
    std::string fluxApFile_{};                                                  ///< 空间天气文件路径（若 useFluxApFile_ == true）
    EGeoMagFluxUpdateRate geoMagFluxUpdateRate_{EGeoMagFluxUpdateRate::eDaily}; ///< 地磁通量数据更新频率
    EGeoMagFluxSource geoMagFluxSource_{EGeoMagFluxSource::eKp};                ///< 地磁通量源类型（Kp 或 Ap）
    double geoMagFluxInterpSubSamplingRatio_{1.0};                              ///< 插值时的下采样因子
    /// @}
    /// @}

    
    /// @name 阻力计算方法开关
    /// @{
    bool useApproxAltForDrag_{false};                                           ///< 是否使用近似高度替代真实高度计算大气密度
    ESunPosition sunPosition_{ESunPosition::eTrue};                             ///< 太阳位置（真实/视太阳位置）
    /// @}
};

AST_CORE_API std::string toString(EAtmDensityModel model);
AST_CORE_API errc_t parse(StringView type, EAtmDensityModel &result);


/*! @} */

AST_NAMESPACE_END

