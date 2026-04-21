///
/// @file      testMissionCommandLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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

#include "AstCore/MissionCommandLoader.hpp"
#include "AstCore/MissionCommand.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/ValXMLLoader.hpp"
#include "AstScript/ValDict.hpp"
#include "AstTest/Test.h"


AST_USING_NAMESPACE;


TEST(MissionCommandLoaderTest, LoadValue)
{
    SharedPtr<Value> value;
    std::vector<std::string> files = aTestGetConfigStringVector("STK_VALUE_FILES");
    for(auto& file: files){
        printf("loading file: %s\n", file.c_str());
        errc_t rc = aLoadValue(file, value);
        EXPECT_TRUE(value != nullptr);
        EXPECT_EQ(rc, eNoError);
        printf("loaded file: %s\n", file.c_str());
        if(value)
        {
            auto dict = value->toValDict();
            if(dict)
            {
                std::string dictStr = dict->toJsonString(2);
                printf("%s\n", dictStr.c_str());
            }
        }
    }
}

TEST(MissionCommandLoaderTest, LoadInitialState)
{
    InitialState initialState;
    std::vector<std::string> files = aTestGetConfigStringVector("STK_INITIALSTATE_FILES");
    for(auto& file: files){
        printf("loading file: %s\n", file.c_str());
        errc_t rc = aLoadInitialState(file, initialState);
        EXPECT_EQ(rc, eNoError);
        printf("loaded file: %s\n", file.c_str());
    }
}


TEST(MissionCommandLoaderTest, LoadMissionCommand)
{
    SharedPtr<MissionCommand> missionCommand;
    std::vector<std::string> files = aTestGetConfigStringVector("STK_SEQUENCE_FILES");
    for(auto& file: files){
        printf("loading file: %s\n", file.c_str());
        errc_t rc = aLoadMissionCommand(file, missionCommand);
        EXPECT_TRUE(missionCommand != nullptr);
        EXPECT_EQ(rc, eNoError);
        printf("loaded file: %s\n", file.c_str());
    }
}

TEST(MissionCommandLoaderTest, LoadPropagate)
{
    SharedPtr<MissionCommand> missionCommand;
    std::vector<std::string> files = aTestGetConfigStringVector("STK_PROPAGATE_FILES");
    for(auto& file: files){
        printf("loading file: %s\n", file.c_str());
        errc_t rc = aLoadMissionCommand(file, missionCommand);
        EXPECT_TRUE(missionCommand != nullptr);
        EXPECT_EQ(rc, eNoError);
        printf("loaded file: %s\n", file.c_str());
    }
}


GTEST_MAIN()
