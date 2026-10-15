///
/// @file      testAtmospherePlugin.cpp
/// @brief     大气模型插件端到端测试
/// @details   验证"扫描目录 → 读元数据建索引 → 按名字创建 → 数值正确"整条链路。
///            插件由 plugins/AstPluginAtmosphere 构建
/// @author    axel
/// @date      2026-10-14
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "ast/Test.hpp"
#include "ast/PluginRegistry.hpp"
#include "ast/AtmospherePlugin.hpp"
#include "ast/DragForce.hpp"
#include "ast/Atmosphere.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/GeodeticPoint.hpp"
#include "ast/Literals.hpp"
#include "ast/Environment.hpp"
#include "ast/IO.hpp"
#include "ast/ConstantSpaceWeather.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

AST_USING_NAMESPACE

TEST(AtmospherePlugin, DiscoverAndCreate)
{
    EXPECT_EQ(aScanPlugins(), eNoError);

    PluginRegistry* registry = PluginRegistry::Instance();
    if (registry->pluginCount() == 0 && !aIsCI())
    {
        ast_printf("未发现任何插件，跳过大气插件端到端测试");
        GTEST_SKIP();
        return;
    }

    // 1. 不实例化任何对象即可列出插件提供的模型名
    std::vector<std::string> names;
    aGetPluginAtmosphereModelNames(names);
    EXPECT_TRUE(std::find(names.begin(), names.end(), std::string("Exponential")) != names.end());

    // 2. 按名字创建（内置模型不走本接口）
    ScopedPtr<SpaceWeatherProvider> spaceWeatherProvider = new ConstantSpaceWeather();
    Atmosphere* atmosphere = aCreateAtmosphereFromPlugin("Exponential", spaceWeatherProvider);
    EXPECT_NE(atmosphere, nullptr);
    if (!atmosphere)
        return;

    // 3. 密度应符合插件实现的指数律：rho = 1.225 * exp(-h / 8500)
    auto earth = aGetEarth();
    EXPECT_NE(earth, nullptr);
    if (!earth)
        return;

    BodyShape* shape = earth->getShape();
    auto tp = "2026-10-13 00:00:00"_utc;

    for (double alt = 0.0; alt <= 200_km; alt += 20_km)
    {
        GeodeticPoint lla(0.0, 0.0, alt);
        Vector3d pos;
        shape->transform(lla, pos);

        const double density = atmosphere->getDensity(tp, pos);
        const double expected = 1.225 * std::exp(-alt / 8500.0);

        EXPECT_TRUE(!std::isnan(density));
        EXPECT_NEAR(density, expected, expected * 1e-6);
    }

    // 4. 未知名字应返回 nullptr
    ScopedPtr<SpaceWeatherProvider> spaceWeatherProvider2 = new ConstantSpaceWeather();
    EXPECT_TRUE(aCreateAtmosphereFromPlugin("NoSuchModel", spaceWeatherProvider2) == nullptr);
}

GTEST_MAIN()
