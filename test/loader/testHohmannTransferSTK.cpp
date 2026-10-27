///
/// @file      testHohmannTransferSTK.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-26
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
#include "ast/PluginRegistry.hpp"
#include "ast/ScriptExecutorProvider.hpp"
#include "ast/ScenarioLoader.hpp"
#include "ast/Scenario.hpp"
#include "ast/Satellite.hpp"
#include "ast/RTTIAPI.hpp"
#include "ast/MotionMissionCommand.hpp"
#include "ast/Sequence.hpp"
#include "ast/Literals.hpp"
#include "ast/OrbitElement.hpp"

#include <string>

AST_USING_NAMESPACE

MotionMissionCommand* loadScenarioAndGetMissionCommand(const char* scenarioDir, Scenario& scenario, StringView satelliteName)
{
    errc_t rc = aLoadScenario(scenarioDir, scenario);
    EXPECT_EQ(rc, eNoError);
    if (rc != eNoError)
        return nullptr;
    Satellite* satellite = aFindChild<Satellite*>(&scenario, satelliteName);
    EXPECT_TRUE(satellite != nullptr);
    if (satellite == nullptr)
        return nullptr;
    EXPECT_EQ(satellite->getName(), satelliteName);

    MotionProfile* motionProfile = satellite->getMotionProfile();
    EXPECT_TRUE(motionProfile != nullptr);

    MotionMissionCommand* motionMissionCommand = dynamic_cast<MotionMissionCommand*>(motionProfile);
    EXPECT_TRUE(motionMissionCommand != nullptr);
    return motionMissionCommand;
}

TEST(HohmannTransferSTKTest, Load)
{
    const char scenarioDir[] = "./STK/Scenarios/testHohmannTransferSTK";
    const std::string path = aTestDataDirGet() + "/" + scenarioDir;
    std::printf("path: %s\n", path.c_str());
    SharedPtr<Scenario> scenario = new Scenario();
    MotionMissionCommand* motionMissionCommand = loadScenarioAndGetMissionCommand(path.c_str(), *scenario.get(), "Satellite2");
    ASSERT_TRUE(motionMissionCommand != nullptr);

    auto& sequence = motionMissionCommand->getSequence();
    auto& commands = sequence.getCommands();
    for(auto& command : commands)
    {
        std::printf("command: %s\n", command->getName().c_str());
    }
    auto segment_Hohmann_Transfer = sequence.getSegmentByPath("Hohmann_Transfer");
    ASSERT_TRUE(segment_Hohmann_Transfer != nullptr);
    auto segment_Prop_To_First_Burn = sequence.getSegmentByPath("Hohmann_Transfer.Prop_To_First_Burn");
    ASSERT_TRUE(segment_Prop_To_First_Burn != nullptr);
}

TEST(HohmannTransferSTKTest, execute)
{
#if !defined(_WIN32)
    // 场景的瞄准序列通过 JScript 脚本工具设置初值：Windows 上由 Active Scripting 执行，
    // 其他平台由 AstPluginJavaScript 运行时插件提供可移植的 JavaScript 引擎
    EXPECT_EQ(aScanPlugins(), eNoError);
#if !defined(AST_WITH_DUKTAPE)
    if(aPluginRegistry().find<IScriptExecutorProvider>("JavaScript") == nullptr)
    {
        GTEST_SKIP() << "JScript requires Active Scripting on Windows or the AstPluginJavaScript plugin on other platforms";
        return;
    }
#endif
#endif
    const char scenarioDir[] = "./STK/Scenarios/testHohmannTransferSTK";
    const std::string path = aTestDataDirGet() + "/" + scenarioDir;
    std::printf("path: %s\n", path.c_str());
    SharedPtr<Scenario> scenario = new Scenario();
    MotionMissionCommand* motionMissionCommand = loadScenarioAndGetMissionCommand(path.c_str(), *scenario.get(), "Satellite2");
    ASSERT_TRUE(motionMissionCommand != nullptr);
    auto& sequence = motionMissionCommand->getSequence();
    errc_t rc = sequence.execute();
    EXPECT_EQ(rc, eNoError);

    auto scstate = sequence.getOutputState();
    ASSERT_NE(scstate, nullptr);
    auto frame = scstate->getFrame();
    ASSERT_NE(frame, nullptr);
    std::printf("frame: '%s'\n", frame->getRepresentation().c_str());
    EXPECT_EQ(frame->getRepresentation(), "Earth Inertial");

    Vector3d finalPosExpected{-10000.0047586389737262_km, 24.4737535445015517_km, -22.0402817260195718_km};
    Vector3d finalVelExpected{-0.0069737198812031_km/s, -5.5488142147491182_km/s, -3.0117528619683362_km/s};
    CartState finalState;
    scstate->getState(finalState);
    EXPECT_NEAR(finalState.pos()[0], finalPosExpected[0], 3e-1);
    EXPECT_NEAR(finalState.pos()[1], finalPosExpected[1], 3e-1);
    EXPECT_NEAR(finalState.pos()[2], finalPosExpected[2], 3e-1);
    EXPECT_NEAR(finalState.vel()[0], finalVelExpected[0], 3e-4);
    EXPECT_NEAR(finalState.vel()[1], finalVelExpected[1], 3e-4);
    EXPECT_NEAR(finalState.vel()[2], finalVelExpected[2], 3e-4);
}


GTEST_MAIN()



