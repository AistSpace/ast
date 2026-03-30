///
/// @file      JsonSerializer.hpp
/// @brief     JSON序列化器
/// @author    axel
/// @date      2026-03-30
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#pragma once

#include "Serializer.hpp"

AST_NAMESPACE_BEGIN

/// @brief JSON序列化器
class AST_SERDE_API JsonSerializer : public Serializer {
public:
    err_t serialize(Object* object, std::string& output) override;
    err_t deserialize(const std::string& input, Object* object) override;
};

AST_NAMESPACE_END