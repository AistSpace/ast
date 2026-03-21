///
/// @file      HPOP.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-16
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
#include "AstUtil/Constants.h"
#include "AstCore/CelestialBody.hpp"
#include <string>
#include <vector>


AST_NAMESPACE_BEGIN

/*!
    @addtogroup Propagator
    @{
*/

class HPOPEquation; 
class ODEIntegrator;

enum class ESolidTideType{
    eFull,
    ePermanentOnly,
    eNone,
};

enum class EAtmDensityModel
{
    e1976Standard,
    eJacchiaRoberts,
    eNRLMSISE2000,
    eMSISE1990,
    eMSIS1986,
};

enum class EGeoMagneticFluxSource
{
    eKp,
    eAp,
};

enum class EGeoMagneticFluxUpdateMethod
{
    eDaily,
    e3Hourly,
    e3HourlyInterp,
    e3HourlyCubicSpline,
};

enum class ESunPosition
{
    eTrue,
    eApparentSunToTrueCB,
    eApparent,
};

enum class EShadowModel
{
    eNone,
    eCylindrical,
    eDualCone,
};

/// @brief 高精度轨道预报力模型
class HPOPForceModel
{
public:
    struct Gravity
    {
        std::string model_{};                                  ///< 中心天体重力场模型(模型名称或者引力场文件路径)
        int maxDegree_{2};                                     ///< 中心天体重力场计算阶数
        int maxOrder_{0};                                      ///< 中心天体重力场计算次数
        // @warning 下面这些参数的相关计算逻辑尚未实现 
        bool useGravSecularVariations_{false};                 ///< 是否考虑引力场的长期变化
        ESolidTideType solidTideType_
            {ESolidTideType::eNone};                           ///< 潮汐类型
        bool includeTimeDependSolidTides_{false};              ///< 是否考虑时间依赖的潮汐
        double minAmplitudeSolidTides_{0.0};                   ///< 最小潮汐振幅
        bool truncateSolidTides_{false};                       ///< 是否截断潮汐
        bool useSolidTides_{false};                            ///< 是否使用潮汐
        int maxDegreeOceanTides_{2};                           ///< 海洋潮汐计算阶数
        int maxOrderOceanTides_{0};                            ///< 海洋潮汐计算次数
        double minAmplitudeOceanTides_{0.0};                   ///< 最小海洋潮汐振幅
    };
    struct Drag
    {
        // @warning 下面这些参数的相关计算逻辑尚未实现
        EAtmDensityModel atmDensityModel_
            {EAtmDensityModel::e1976Standard};                      ///< 大气密度模型
        double atmBlendingRange_{0.0};                              ///< 大气混合范围
        bool useFluxApFile_{false};                                 ///< 是否使用空间天气文件
        std::string fluxApFile_{};                                  ///< 空间天气文件路径
        EGeoMagneticFluxSource geoMagneticFluxSource_
            {EGeoMagneticFluxSource::eKp};                          ///< 地磁通量源类型
        EGeoMagneticFluxUpdateMethod geoMagneticFluxUpdateMethod_
            {EGeoMagneticFluxUpdateMethod::eDaily};                 ///< 地磁通量更新方法类型
        double geoMagneticFluxInterpSubSamplingRatio_{1.0};         ///< 地磁通量插值下采样比
        double dragCoefficient_{0.0};                               ///< 阻力系数
        double areaMassRatio_{0.0};                                 ///< 面积质量比
        // DragCorrectionType       
        double f10p7Average_{0.0};                                  ///< 平均F10.7
        double f10p7Daily_{0.0};                                    ///< 每日F10.7
        double kp_{0.0};                                            ///< Kp
        bool useApproxAltForDrag_{false};                           ///< 是否使用近似高度计算阻力
        bool useApparentSunForDrag_{false};                         ///< 是否使用 apparent sun 计算阻力
        // DensityType
    };
    struct SRP
    {
        // @warning 下面这些参数的相关计算逻辑尚未实现
        double coefficient_{0.0};                            ///< 太阳辐射压系数
        double areaMassRatio_{0.0};                          ///< 面积质量比
        // ReflectionModel
        // CorrectionType
        ESunPosition sunPosition_
            {ESunPosition::eTrue};                           ///< 太阳位置
        bool detectShadowBoundaries_{false};                 ///< 是否检测阴影边界
        EShadowModel shadowModel_
            {EShadowModel::eNone};                           ///< 阴影模型类型
        std::vector<HCelestialBody> eclipsingBodies_;        ///< 遮挡天体列表
        double atmAltForEclipse_{0.0};                       ///< 用于计算阴影边界的大气高度
    };
    struct ThirdBody
    {
        // @warning 下面这些参数的相关计算逻辑尚未实现
        HCelestialBody body_;                            ///< 天体
    };
    using ThirdBodyList = std::vector<ThirdBody>;
public:
    bool            useDrag_{false};                    ///< 是否使用阻力模型
    bool            useSRP_{false};                     ///< 是否使用太阳辐射压模型
    bool            useMoonGravity_{false};             ///< 是否使用月球引力
    Gravity         gravity_;                           ///< 中心天体重力场
    Drag            drag_;                              ///< 阻力模型
    SRP             srp_;                               ///< 太阳辐射压模型
    double          moonGravity_{kMoonGrav};            ///< 月球引力系数
    ThirdBodyList   thirdBodies_;                       ///< 三体引力
};

/// @brief 高精度轨道预报接口类
class AST_CORE_API HPOP
{
public:
    HPOP() = default;
    ~HPOP();
public:
    /// @brief 设置力模型
    err_t setForceModel(const HPOPForceModel& forcemodel);

    /// @brief 设置积分器
    void setIntegrator(ODEIntegrator* integrator);

    /// @brief 获取积分器
    ODEIntegrator* getIntegrator() const;

    /// @brief 轨道预报
    /// 考虑到有停止条件，所以预报结束时间同时也是一个输出参数
    /// @param[in]      startTime   预报起始时间
    /// @param[in,out]  targetTime  预报结束时间
    /// @param[in,out]  position    输出位置向量
    /// @param[in,out]  velocity    输出速度向量
    /// @return err_t  错误码
    err_t propagate(const TimePoint& startTime, TimePoint& targetTime, Vector3d& position, Vector3d& velocity);

    /// @brief 初始化
    err_t initialize();
protected:
    HPOPEquation* equation_{nullptr};               ///< 高精度轨道预报方程
    mutable ODEIntegrator* integrator_{nullptr};    ///< 高精度轨道预报积分器
    HPOPForceModel forcemodel_;                     ///< 力模型配置
};


/*! @} */

AST_NAMESPACE_END
