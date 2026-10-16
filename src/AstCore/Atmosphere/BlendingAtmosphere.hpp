///
/// @file      BlendingAtmosphere.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-13
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
#include "AtmosphereBase.hpp"
#include "AstUtil/ScopedPtr.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


/// @brief 主/低高度大气模型的过渡带中心高度默认值
/// @details 各高高度模型的适用下限大致在 86~120 km（1976标准 86 km、JacchiaRoberts 90 km、Jacchia71 100 km、DTM2012 120 km），
///    低高度模型（MSISE1990/NRLMSISE2000）自 0 km 起适用。取 100 km 作为默认过渡中心，仅在没有更确切依据时使用。
constexpr double kDefaultBlendAltitude = 100e3;


/// @brief 主/低高度大气密度模型的混合过渡模型
/// @details 在过渡高度附近，将"主大气模型"（高高度模型）与"低高度模型"按高度线性混合：
///    - 高度 ≤ blendAltitude - blendRange/2：全部使用低高度模型；
///    - 高度 ≥ blendAltitude + blendRange/2：全部使用主模型；
///    - 过渡带内：对两者按高度线性加权（低高度偏重低高度模型，高高度偏重主模型）。
class AST_CORE_API BlendingAtmosphere : public Atmosphere
{
public:
    /// @brief 构造混合过渡大气模型
    /// @param mainModel 主（高高度）大气模型
    /// @param lowAltModel 低高度大气模型
    /// @param blendAltitude 过渡带中心高度（单位：米）
    /// @param blendRange 过渡带宽度（单位：米），≤0 时不做过渡，直接按 blendAltitude 二选一
    BlendingAtmosphere(Atmosphere* mainModel, Atmosphere* lowAltModel, double blendAltitude, double blendRange);

    BlendingAtmosphere(Atmosphere* mainModel, Atmosphere* lowAltModel);

    ~BlendingAtmosphere() override = default;

public: // 重载行为
    double getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const override;
    Frame* getFrame() const override;
    BodyShape* getShape() const override;
public:
    /// @brief 主（高高度）大气模型
    Atmosphere* mainModel() const { return mainModel_; }
    void setMainModel(Atmosphere* mainModel) { mainModel_ = mainModel; }

    /// @brief 低高度大气模型
    Atmosphere* lowAltModel() const { return lowAltModel_; }
    void setLowAltModel(Atmosphere* lowAltModel) { lowAltModel_ = lowAltModel; }

    /// @brief 过渡带中心高度
    double blendAltitude() const { return blendAltitude_; }
    void setBlendAltitude(double blendAltitude) { blendAltitude_ = blendAltitude; }

    /// @brief 过渡带宽度
    double blendRange() const { return blendRange_; }
    void setBlendRange(double blendRange) { blendRange_ = blendRange; }
private:
    ScopedPtr<Atmosphere> mainModel_{};           ///< 主（高高度）大气模型
    ScopedPtr<Atmosphere> lowAltModel_{};         ///< 低高度大气模型
    double blendAltitude_{kDefaultBlendAltitude}; ///< 过渡带中心高度 [m]
    double blendRange_{0.0};                      ///< 过渡带宽度 [m]
};


/*! @} */

AST_NAMESPACE_END
