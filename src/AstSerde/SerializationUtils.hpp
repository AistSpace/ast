///
/// @file      SerializationUtils.hpp
/// @brief     序列化工具类
/// @author    axel
/// @date      2026-03-30
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#pragma once

#include "Serializer.hpp"

AST_NAMESPACE_BEGIN

/// @brief 序列化工具类
class AST_SERDE_API SerializationUtils {
public:
    /// @brief 序列化对象到指定格式
    /// @param object 要序列化的对象
    /// @param format 序列化格式
    /// @param output 输出字符串
    /// @return 错误码
    static err_t serialize(Object* object, ESerializationFormat format, std::string& output);
    
    /// @brief 从指定格式反序列化对象
    /// @param input 输入字符串
    /// @param format 序列化格式
    /// @param object 要反序列化到的对象
    /// @return 错误码
    static err_t deserialize(const std::string& input, ESerializationFormat format, Object* object);
};

AST_NAMESPACE_END