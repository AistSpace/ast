///
/// @file      Serializer.hpp
/// @brief     序列化器基类
/// @author    axel
/// @date      2026-03-30
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Object.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 序列化格式枚举
enum class ESerializationFormat {
    eJson,       ///< JSON格式
    eXml,        ///< XML格式
    eCpp,        ///< C++代码
    eJava,       ///< Java代码
    ePython      ///< Python代码
};

/// @brief 序列化器基类
class AST_UTIL_API Serializer {
public:
    virtual ~Serializer() = default;
    
    /// @brief 序列化对象
    /// @param object 要序列化的对象
    /// @param output 输出字符串
    /// @return 错误码
    virtual errc_t serialize(Object* object, std::string& output) = 0;
    
    /// @brief 反序列化对象
    /// @param input 输入字符串
    /// @param object 要反序列化到的对象
    /// @return 错误码
    virtual errc_t deserialize(const std::string& input, Object* object) = 0;
};

AST_NAMESPACE_END