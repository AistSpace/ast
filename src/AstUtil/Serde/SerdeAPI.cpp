///
/// @file      SerdeAPI.cpp
/// @brief     序列化模块的API实现
/// @author    axel
/// @date      2026-03-30
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#include "SerdeAPI.hpp"
#include "Serde.hpp"
#include "AstUtil/IO.hpp"

AST_NAMESPACE_BEGIN

errc_t aObjectToCppCode(Object* object, std::string& cppcode) {
    return SerializationUtils::serialize(object, ESerializationFormat::eCpp, cppcode);
}

errc_t aObjectToCppFile(Object *object, StringView cppfilepath)
{
    std::string cppcode;
    aObjectToCppCode(object, cppcode);
    FILE* file = ast_fopen(std::string(cppfilepath).c_str(), "w");
    if (file == nullptr)
    {
        return eErrorInvalidFile;
    }
    fwrite(cppcode.c_str(), 1, cppcode.size(), file);
    fclose(file);
    return eNoError;
}

errc_t aObjectSerialize(Object* object, ESerializationFormat format, std::string& output) {
    return SerializationUtils::serialize(object, format, output);
}

errc_t aObjectDeserialize(const std::string& input, ESerializationFormat format, Object* object) {
    return SerializationUtils::deserialize(input, format, object);
}

AST_NAMESPACE_END