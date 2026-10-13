///
/// @file      testBlendingAtmosphere.cpp
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

#include "ast/Test.hpp"
#include "ast/BlendingAtmosphere.hpp"
#include "ast/USSA1976.hpp"
#include "ast/NRLMSIS00.hpp"
#include "ast/RunTime.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/Literals.hpp"
#include "ast/GeodeticPoint.hpp"
#include "ast/IO.hpp"

AST_USING_NAMESPACE

namespace
{
/// @brief 返回恒定密度的大气模型桩，用于校验混合逻辑
class ConstantDensityAtmosphere : public Atmosphere
{
public:
    ConstantDensityAtmosphere(Frame* frame, double density)
        : frame_(frame), density_(density) {}

    double getDensity(const TimePoint&, const Vector3d&) const override { return density_; }
    Frame* getFrame() const override { return frame_; }
private:
    Frame* frame_{nullptr};
    double density_{0.0};
};

/// @brief 计算给定地心高度（单位：米）处的大气密度
double densityAt(const Atmosphere& atmosphere, BodyShape* shape, const TimePoint& tp, double altitude)
{
    GeodeticPoint lla(0.0, 0.0, altitude);
    Vector3d pos;
    shape->transform(lla, pos);
    return atmosphere.getDensity(tp, pos);
}
}


/// @brief 校验过渡带内的线性混合以及带外的模型选择
TEST(BlendingAtmosphereTest, LinearBlend)
{
    auto earth = aGetEarth();
    auto shape = earth->getShape();
    Frame* frame = earth->getFrameFixed();

    const double lowDensity  = 1.0e-8;   // 低高度模型密度 [kg/m^3]
    const double mainDensity = 3.0e-8;   // 主模型密度 [kg/m^3]
    const double blendAltitude = 100_km; // 过渡带中心高度 [m]
    const double blendRange    = 20_km;  // 过渡带宽度 [m]

    BlendingAtmosphere atmosphere(new ConstantDensityAtmosphere(frame, mainDensity),
                                  new ConstantDensityAtmosphere(frame, lowDensity),
                                  blendAltitude, blendRange);
    auto tp = "2026-10-13 00:00:00"_utc;

    EXPECT_EQ(atmosphere.getFrame(), frame);
    EXPECT_DOUBLE_EQ(atmosphere.blendAltitude(), blendAltitude);
    EXPECT_DOUBLE_EQ(atmosphere.blendRange(), blendRange);

    // 过渡带以下（< 90 km）：使用低高度模型
    EXPECT_NEAR(densityAt(atmosphere, shape, tp, 80_km), lowDensity, 1e-20);
    // 过渡带以上（> 110 km）：使用主模型
    EXPECT_NEAR(densityAt(atmosphere, shape, tp, 120_km), mainDensity, 1e-20);
    // 过渡带中心（100 km）：两者各占 0.5
    EXPECT_NEAR(densityAt(atmosphere, shape, tp, 100_km), 0.5 * (lowDensity + mainDensity), 1e-20);
    // 过渡带内偏低位（95 km）：权重 0.25(主)/0.75(低)
    EXPECT_NEAR(densityAt(atmosphere, shape, tp, 95_km), 0.25 * mainDensity + 0.75 * lowDensity, 1e-20);
    // 过渡带内偏高位（105 km）：权重 0.75(主)/0.25(低)
    EXPECT_NEAR(densityAt(atmosphere, shape, tp, 105_km), 0.75 * mainDensity + 0.25 * lowDensity, 1e-20);
}


/// @brief 过渡带宽度为 0 时，退化为按中心高度的二选一
TEST(BlendingAtmosphereTest, ZeroRangeIsStep)
{
    auto earth = aGetEarth();
    auto shape = earth->getShape();
    Frame* frame = earth->getFrameFixed();

    const double lowDensity  = 1.0e-8;
    const double mainDensity = 3.0e-8;

    BlendingAtmosphere atmosphere(new ConstantDensityAtmosphere(frame, mainDensity),
                                  new ConstantDensityAtmosphere(frame, lowDensity),
                                  100_km, 0.0);
    auto tp = "2026-10-13 00:00:00"_utc;

    EXPECT_NEAR(densityAt(atmosphere, shape, tp, 80_km), lowDensity, 1e-20);
    EXPECT_NEAR(densityAt(atmosphere, shape, tp, 120_km), mainDensity, 1e-20);
}


/// @brief 使用真实模型（1976 标准大气 + NRLMSISE-2000）做冒烟测试
TEST(BlendingAtmosphereTest, RealModels)
{
    auto earth = aGetEarth();
    auto shape = earth->getShape();
    Frame* frame = earth->getFrameFixed();

    BlendingAtmosphere atmosphere(new USSA1976(frame, shape),
                                  new NRLMSIS00(frame, shape, 150, 150, 15),
                                  100_km, 20_km);
    auto tp = "2026-10-13 00:00:00"_utc;

    for(double alt = 80_km; alt <= 200_km; alt += 10_km)
    {
        double density = densityAt(atmosphere, shape, tp, alt);
        posix::printf("高度: %lf km, 混合大气密度: %g kg/m^3\n", alt / 1_km, density);
        EXPECT_TRUE(!std::isnan(density));
        EXPECT_TRUE(density > 0.0);
    }
}

GTEST_MAIN()
