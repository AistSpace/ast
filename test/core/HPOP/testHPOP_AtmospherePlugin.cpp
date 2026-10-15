///
/// @file      testHPOP_AtmospherePlugin.cpp
/// @brief     大气阻力插件的端到端测试
/// @details   验证"阻力配置里以名字指定大气模型 → 插件提供实现 → HPOP 积分"整条链路：
///            1. 模型名字能从插件元数据里查到；
///            2. HPOP 取到的是插件的大气模型，而不是静默回退到内置 NRLMSISE2000；
///            3. 低轨上该大气模型产生可观测的阻力衰减。
///            插件由 plugins/AstPluginAtmosphere 构建（需要 with_plugins=y），
///            未构建插件时跳过；CI 上构建全部目标，插件必然存在。
/// @author    axel
/// @date      2026-10-15
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


#include "ast/HPOP.hpp"
#include "ast/HPOPForceModel.hpp"
#include "ast/AtmospherePlugin.hpp"
#include "ast/PluginRegistry.hpp"
#include "ast/DragForce.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/EOP.hpp"
#include "ast/ODEVarStepIntegrator.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/SpacecraftParam.hpp"
#include "ast/RunTime.hpp"
#include "ast/Environment.hpp"
#include "ast/Test.hpp"
#include "ast/Literals.hpp"
#include "ast/IO.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

AST_USING_NAMESPACE
using namespace literals;

class HPOPAtmospherePluginTest : public ::testing::Test
{
    void SetUp() override
    {
        aInitialize();
        aDataContext_GetEOP()->unload();  // 卸载EOP数据，确保测试不受EOP修正数据的影响
    }

    void TearDown() override
    {
        aUninitialize();
    }
};

namespace {

/// @brief 一次预报的末状态
struct PropagationResult
{
    errc_t   err{eNoError};
    Vector3d pos{};
    Vector3d vel{};
    double   elapsed{0.0};   ///< 实际预报时长 [s]
};

/// @brief 用固定的力模型/航天器参数预报给定初值，只改变大气模型配置
/// @param model   大气模型标识（内置枚举或模型名字，可以是插件模型）
/// @param useDrag 是否启用大气阻力
PropagationResult propagateOnce(
    const AtmosphereModelId& model, bool useDrag,
    const TimePoint& start, const TimePoint& end,
    const Vector3d& pos0, const Vector3d& vel0)
{
    HPOPForceModel forceModel;
    forceModel.gravity().maxDegree_ = 21;
    forceModel.gravity().maxOrder_ = 21;
    forceModel.gravity().model_ = "EGM2008";
    forceModel.gravity().useSecularVariations_ = false;
    forceModel.gravity().solidTideType_ = ESolidTideType::eNone;

    forceModel.useDrag(useDrag);
    forceModel.drag().atmDensityModel_ = model;
    forceModel.drag().useApproxAltForDrag_ = false;
    forceModel.drag().f10p7Average_ = 150;
    forceModel.drag().f10p7Daily_ = 150;
    forceModel.drag().kp_ = 3.0;

    SpacecraftParam scParam;
    scParam.setDryMass(1000.0);
    scParam.setFuelMass(0.0);
    scParam.setDragArea(20);
    scParam.setCd(2.2);

    HPOP propagator;
    PropagationResult result;
    result.err = propagator.setForceModel(forceModel);
    if (result.err)
        return result;
    propagator.setSpacecraftParam(scParam);

    auto varStepIntegrator = aobject_cast<ODEVarStepIntegrator*>(propagator.getIntegrator());
    if (varStepIntegrator)
    {
        varStepIntegrator->setUseFixedStep(true);
        varStepIntegrator->setStepSize(60);
    }

    result.pos = pos0;
    result.vel = vel0;
    // 事件触发时 propagate() 会把实际到达的时刻回写到 stopTime 上，这里用副本隔离
    TimePoint stopTime = end;
    result.err = propagator.propagate(start, stopTime, result.pos, result.vel);
    result.elapsed = stopTime - start;
    return result;
}

/// @brief 200 km 赤道圆轨道初值
void initialLowOrbit(Vector3d& pos0, Vector3d& vel0)
{
    auto earth = aGetEarth();
    const double radius = kEarthRadius + 200_km;
    pos0 = Vector3d{radius, 0.0, 0.0};
    vel0 = Vector3d{0.0, std::sqrt(earth->getGM() / radius), 0.0};
}

/// @brief 等效高度：半长轴折算的高度 a - Re [km]
/// @details 近圆轨道上瞬时高度被 J2 短周期项主导（无阻力时也能差十几公里），
///    只有半长轴是稳定的衰减量，所以各模型之间比的是等效高度
double equivalentAltitude(const Vector3d& pos, const Vector3d& vel, double gm)
{
    OrbElem elem;
    aCartToOrbElem(pos, vel, gm, elem);
    return (elem.getSMA() - kEarthRadius) / 1_km;
}

/// @brief 打印一次预报的等效高度与相对初值的衰减
/// @param name      模型名
/// @param altitude0 初值等效高度 [km]
/// @param note      追加在行尾的备注（可为空）
void printDecay(const char* name, const PropagationResult& r,
                double altitude0, double gm, const char* note = nullptr)
{
    const double altitude = equivalentAltitude(r.pos, r.vel, gm);
    posix::printf("%-20s err=%d t=%7.1fs 等效高度 %8.3f -> %8.3f km  衰减 %+7.3f km %s\n",
                  name, (int)r.err, r.elapsed, altitude0, altitude,
                  altitude - altitude0, note ? note : "");
}

} // namespace


/// @brief 测试插件提供的大气阻力模型
TEST_F(HPOPAtmospherePluginTest, Drag_PluginAtmosphere)
{
    // 1. 插件发现：aInitialize() 里已经扫描过一次
    EXPECT_EQ(aScanPlugins(), eNoError);

    std::vector<std::string> modelNames;
    aGetPluginAtmosphereModelNames(modelNames);
    const bool hasExponential = std::find(modelNames.begin(), modelNames.end(), std::string("Exponential")) != modelNames.end();

    if (!hasExponential && !aIsCI())
    {
        // 本地没有构建插件（with_plugins=n 或没编 AstPluginAtmosphere）时跳过，
        // CI 上构建全部目标，插件必然存在
        posix::printf("未发现大气插件 'Exponential'，跳过测试。\n");
        GTEST_SKIP();
        return;
    }
    EXPECT_TRUE(hasExponential);

    const auto start = "2026-10-15 00:00:00"_utc;
    const auto end   = start + 1_h;

    Vector3d pos0, vel0;
    initialLowOrbit(pos0, vel0);

    // 2. 同一初值、同一力模型，只有大气模型配置不同
    const auto noDrag    = propagateOnce(EAtmosphereModel::eNone, false, start, end, pos0, vel0);
    const auto pluginAtm = propagateOnce(AtmosphereModelId("Exponential"), true, start, end, pos0, vel0);
    const auto builtin   = propagateOnce(EAtmosphereModel::eNRLMSISE2000, true, start, end, pos0, vel0);

    EXPECT_EQ(noDrag.err, eNoError);
    EXPECT_EQ(pluginAtm.err, eNoError);
    EXPECT_EQ(builtin.err, eNoError);
    if (noDrag.err || pluginAtm.err || builtin.err)
        return;

    // 三次预报都完整走完 1 小时（没有被事件或积分失败截断）
    EXPECT_NEAR(noDrag.elapsed, 1_h, 1e-6);
    EXPECT_NEAR(pluginAtm.elapsed, 1_h, 1e-6);
    EXPECT_NEAR(builtin.elapsed, 1_h, 1e-6);

    auto earth = aGetEarth();
    const double gm = earth->getGM();
    const double altitude0 = equivalentAltitude(pos0, vel0, gm);
    posix::printf("初值 等效高度 %.3f km，预报 1 小时:\n", altitude0);
    printDecay("无阻力(对照)", noDrag, altitude0, gm);
    printDecay("Exponential(插件)", pluginAtm, altitude0, gm);
    printDecay("NRLMSISE2000", builtin, altitude0, gm);

    posix::printf("pos delta [km]: plugin vs noDrag %.3f, plugin vs NRLMSISE2000 %.3f, NRLMSISE2000 vs noDrag %.3f\n",
           (pluginAtm.pos - noDrag.pos).norm() / 1_km,
           (pluginAtm.pos - builtin.pos).norm() / 1_km,
           (builtin.pos - noDrag.pos).norm() / 1_km);

    // 3. 插件大气确实作用于轨道：相对于无阻力解，位置偏移数公里
    EXPECT_GT((pluginAtm.pos - noDrag.pos).norm(), 1_km);
    // 对照：内置模型同样产生偏移
    EXPECT_GT((builtin.pos - noDrag.pos).norm(), 1_km);

    // 4. 插件模型确实被采用：模型名解析不到插件时 HPOP 会静默回退到内置 NRLMSISE2000，
    //    那种情况下这一次预报会与内置模型逐位一致。
    EXPECT_GT((pluginAtm.pos - builtin.pos).norm(), 100.0);
}


/// @brief 列出每种大气模型在同一条 200 km 轨道上 1 小时的衰减高度
TEST_F(HPOPAtmospherePluginTest, Probe_ModelDecay)
{
    const auto start = "2026-10-15 00:00:00"_utc;
    const auto end   = start + 1_h;

    Vector3d pos0, vel0;
    initialLowOrbit(pos0, vel0);
    auto earth = aGetEarth();
    const double gm = earth->getGM();
    const double altitude0 = equivalentAltitude(pos0, vel0, gm);

    const auto noDrag = propagateOnce(EAtmosphereModel::eNone, false, start, end, pos0, vel0);

    // 兜底模型是 NRLMSISE2000：与它逐位相同，说明该模型没被支持、静默回退了
    const auto nrlmsis = propagateOnce(EAtmosphereModel::eNRLMSISE2000, true, start, end, pos0, vel0);

    const auto report = [&](const std::string& name, const AtmosphereModelId& model) {
        const auto r = propagateOnce(model, true, start, end, pos0, vel0);
        const bool fellBack = (r.pos - nrlmsis.pos).norm() < 1e-6 && name != "NRLMSISE2000";
        printDecay(name.c_str(), r, altitude0, gm, fellBack ? "(回退到 NRLMSISE2000)" : nullptr);
    };

    posix::printf("初值 等效高度 %.3f km，预报 1 小时 (A=20 m^2, Cd=2.2, m=1000 kg, F10.7=150, Kp=3):\n",
                  altitude0);
    printDecay("无阻力(对照)", noDrag, altitude0, gm);
    posix::printf("--- 内置模型 ---\n");
    for (EAtmosphereModel m : {EAtmosphereModel::e1976Standard, EAtmosphereModel::eHarrisPriester,
                               EAtmosphereModel::eJacchia60, EAtmosphereModel::eJacchia70,
                               EAtmosphereModel::eJacchia71, EAtmosphereModel::eJacchiaRoberts,
                               EAtmosphereModel::eMSIS1986, EAtmosphereModel::eMSISE1990,
                               EAtmosphereModel::eNRLMSISE2000, EAtmosphereModel::eCIRA72,
                               EAtmosphereModel::eDTM2012})
    {
        report(toString(m), m);
    }
    posix::printf("--- 插件模型 ---\n");
    std::vector<std::string> pluginNames;
    aGetPluginAtmosphereModelNames(pluginNames);
    if (pluginNames.empty())
        posix::printf("(未发现插件)\n");
    for (const std::string& name : pluginNames)
        report(name, AtmosphereModelId(name));
}


/// @brief 测试以名字指定内置大气模型（名字→枚举的解析分支）
/// @details 与直接给枚举值应当得到逐位相同的预报结果
TEST_F(HPOPAtmospherePluginTest, Drag_BuiltinAtmosphereByName)
{
    const auto start = "2026-10-15 00:00:00"_utc;
    const auto end   = start + 1_h;

    Vector3d pos0, vel0;
    initialLowOrbit(pos0, vel0);

    const auto byName  = propagateOnce(AtmosphereModelId("NRLMSISE2000"), true, start, end, pos0, vel0);
    const auto byValue = propagateOnce(EAtmosphereModel::eNRLMSISE2000, true, start, end, pos0, vel0);

    EXPECT_EQ(byName.err, eNoError);
    EXPECT_EQ(byValue.err, eNoError);
    if (byName.err || byValue.err)
        return;

    EXPECT_NEAR((byName.pos - byValue.pos).norm(), 0.0, 1e-6);
    EXPECT_NEAR((byName.vel - byValue.vel).norm(), 0.0, 1e-9);
}


GTEST_MAIN()
