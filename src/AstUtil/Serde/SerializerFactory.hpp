///
/// @file      SerializerFactory.hpp
/// @brief     序列化工厂类
/// @author    axel
/// @date      2026-03-30
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#pragma once

#include "Serializer.hpp"

AST_NAMESPACE_BEGIN

/// @brief 序列化工厂类
class AST_UTIL_API SerializerFactory {
public:
    /// @brief 创建序列化器
    /// @param format 序列化格式
    /// @return 序列化器指针
    static Serializer* createSerializer(ESerializationFormat format);
    
    /// @brief 销毁序列化器
    /// @param serializer 序列化器指针
    static void destroySerializer(Serializer* serializer);
};

AST_NAMESPACE_END