///
/// @file      testSerializeCpp.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-30
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

#include "AstUtil/SerdeAPI.hpp"
#include "AstUtil/CppSerializer.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/BuilderAPI.hpp"
#include "AstTest/Test.h"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/CelestialBody.hpp"

AST_USING_NAMESPACE

TEST(SerializeCppTest, StateCartesian) {
    if(aIsCI())
        GTEST_SKIP();

    aInitialize();
    auto earth = aGetBody("Earth");
    StateCartesian* state = new StateCartesian();
    state->setStateEpoch(TimePoint::FromUTC(2022, 2, 4, 12, 0, 0));
    state->setFrame(earth->makeFrameICRF());
    state->setX(7000);
    state->setY(8000);
    state->setZ(9000);
    state->setVx(1000);
    state->setVy(2000);
    state->setVz(3000);

    std::string cppcode1;
    std::string cppcode2;
    errc_t rc = aObjectSerialize(state, ESerializationFormat::eCpp, cppcode1);
    EXPECT_EQ(rc, eNoError);
    printf("%s\n", cppcode1.c_str());

    const std::string filepath1 = "./serializeStateCartesian1.cpp";
    const std::string filepath2 = "./serializeStateCartesian2.cpp";
    rc = aObjectToCppFile(state, filepath1);
    EXPECT_EQ(rc, eNoError);
    FILE* file = ast_fopen(filepath1.c_str(), "a");

    std::string str = 
    "#include \"AstUtil/SerdeAPI.hpp\"\n"
    "int main() {\n"
    "    auto obj = " + CppSerializer::getFuncName(state) + "();\n"
    "    aObjectToCppFile(obj, \"" + filepath2 + "\");\n"
    "    return 0;\n"
    "}";
    fwrite(str.c_str(), 1, str.size(), file);
    fclose(file);
    // 编译执行
    Target target("test-serialize");
    rc = target
        .setKind("binary")
        .addFiles({filepath1})
        .addLinkDirs({"."})
        .addLinks({"AstCore", "AstUtil"})
        .addIncludeDirs({"../../../../include"})
        .build();
    EXPECT_EQ(rc, 0);
    rc = target.run();
    EXPECT_EQ(rc, 0);
    
    // 读取filepath2文件内容
    file = ast_fopen(filepath2.c_str(), "r");
    std::vector<char> buffer(10241);
    size_t bytesRead = fread(buffer.data(), 1, 10240, file);
    fclose(file);
    cppcode2 = std::string(buffer.begin(), buffer.begin() + bytesRead);
    printf("%s\n", cppcode2.c_str());
    EXPECT_EQ(cppcode1, cppcode2);
}


GTEST_MAIN()
