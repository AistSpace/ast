///
/// @file      SerdeAPI.cpp
/// @brief     序列化模块的API实现
/// @author    axel
/// @date      2026-03-30
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#include "SerdeAPI.hpp"
#include "AstSerde.hpp"

AST_NAMESPACE_BEGIN

AST_SERDE_CAPI errc_t aObjectToCppCode(Object* object, std::string& cppcode) {
    return SerializationUtils::serialize(object, ESerializationFormat::eCpp, cppcode);
}

AST_SERDE_CAPI errc_t aObjectSerialize(Object* object, ESerializationFormat format, std::string& output) {
    return SerializationUtils::serialize(object, format, output);
}

AST_SERDE_CAPI errc_t aObjectDeserialize(const std::string& input, ESerializationFormat format, Object* object) {
    return SerializationUtils::deserialize(input, format, object);
}

AST_NAMESPACE_END