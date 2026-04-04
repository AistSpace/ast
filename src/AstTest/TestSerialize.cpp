///
/// @file      TestSerialize.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-31
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

#include "TestSerialize.hpp"
#include "AstUtil/Target.hpp"
#include "AstUtil/CppSerializer.hpp"
#include "AstUtil/SerdeAPI.hpp"
#include "AstUtil/IO.hpp"
#include <string>

AST_NAMESPACE_BEGIN

errc_t aTestSerializeCpp(Object *object)
{
    std::string cppcode1;
    std::string cppcode2;
    errc_t rc = aObjectSerialize(object, ESerializationFormat::eCpp, cppcode1);
    printf("%s\n", cppcode1.c_str());

    const std::string filepath1 = "./temp_for_test_serialize1.cpp";
    const std::string filepath2 = "./temp_for_test_serialize2.cpp";
    rc = aObjectToCppFile(object, filepath1);
    // EXPECT_EQ(rc, eNoError);
    FILE* file = ast_fopen(filepath1.c_str(), "a");

    std::string str = 
    "#include \"AstUtil/SerdeAPI.hpp\"\n"
    "int main() {\n"
    "    auto obj = " + CppSerializer::getFuncName(object) + "();\n"
    "    aObjectToCppFile(obj, \"" + filepath2 + "\");\n"
    "    return 0;\n"
    "}";
    fwrite(str.c_str(), 1, str.size(), file);
    fclose(file);
    // 编译执行
    Target target("temp_for_test_serialize");
    rc = target
        .setKind("binary")
        .addFiles({filepath1})
        .addLinkDirs({"."})
        .addLinks({"AstCore", "AstUtil"})
        .addIncludeDirs({"../../../../include"})
        .build();
    if(rc)
        return rc;
    rc = target.run();
    if(rc)
        return rc;
    
    // 读取filepath2文件内容，与cppcode1进行比较
    file = ast_fopen(filepath2.c_str(), "r");
    std::vector<char> buffer(10241);
    size_t bytesRead = fread(buffer.data(), 1, 10240, file);
    fclose(file);
    cppcode2 = std::string(buffer.begin(), buffer.begin() + bytesRead);
    printf("%s\n", cppcode2.c_str());
    if(cppcode1 != cppcode2)
        return -1;
    return 0;
}

AST_NAMESPACE_END

