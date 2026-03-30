///
/// @file      SerializationUtils.cpp
/// @brief     序列化工具类实现
/// @author    axel
/// @date      2026-03-30
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#include "SerializationUtils.hpp"
#include "SerializerFactory.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/Property.hpp"
#include <memory>

AST_NAMESPACE_BEGIN

errc_t SerializationUtils::serialize(Object* object, ESerializationFormat format, std::string& output) {
    if (!object) {
        return eErrorNullInput;
    }
    
    std::unique_ptr<Serializer> serializer(SerializerFactory::createSerializer(format));
    if (!serializer) {
        return eErrorNullPtr;
    }
    
    return serializer->serialize(object, output);
}

errc_t SerializationUtils::deserialize(const std::string& input, ESerializationFormat format, Object* object) {
    if (!object) {
        return eErrorNullInput;
    }
    
    std::unique_ptr<Serializer> serializer(SerializerFactory::createSerializer(format));
    if (!serializer) {
        return eErrorNullPtr;
    }
    
    return serializer->deserialize(input, object);
}



AST_NAMESPACE_END