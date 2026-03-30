///
/// @file      SerializerFactory.cpp
/// @brief     序列化工厂类实现
/// @author    axel
/// @date      2026-03-30
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#include "SerializerFactory.hpp"
#include "JsonSerializer.hpp"
#include "XmlSerializer.hpp"
#include "CppSerializer.hpp"
#include "JavaSerializer.hpp"
#include "PythonSerializer.hpp"

AST_NAMESPACE_BEGIN

Serializer* SerializerFactory::createSerializer(ESerializationFormat format) {
    switch (format) {
        case ESerializationFormat::eJson:
            return new JsonSerializer();
        case ESerializationFormat::eXml:
            return new XmlSerializer();
        case ESerializationFormat::eCpp:
            return new CppSerializer();
        case ESerializationFormat::eJava:
            return new JavaSerializer();
        case ESerializationFormat::ePython:
            return new PythonSerializer();
        default:
            return nullptr;
    }
}

void SerializerFactory::destroySerializer(Serializer* serializer) {
    delete serializer;
}

AST_NAMESPACE_END