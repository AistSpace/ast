///
/// @file      XmlSerializer.cpp
/// @brief     XML序列化器实现
/// @author    axel
/// @date      2026-03-30
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#include "XmlSerializer.hpp"
#include "SerializationUtils.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/Property.hpp"
#include <sstream>

AST_NAMESPACE_BEGIN


err_t XmlSerializer::serialize(Object* object, std::string& output) {
    return -1;
}

err_t XmlSerializer::deserialize(const std::string& input, Object* object) {
    return -1;
}

AST_NAMESPACE_END