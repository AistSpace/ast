///
/// @file      JsonValue.hpp
/// @brief     JSON 值类
/// @details   提供 JSON 值的表示和操作
/// @author    axel
/// @date      2026-04-16
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include <string>
#include <vector>
#include <map>

AST_NAMESPACE_BEGIN

/// @brief JSON 值类型枚举
/// @ingroup ParseFormat
enum class JsonValueType
{
    Null,      ///< null 值
    Boolean,   ///< 布尔值
    Number,    ///< 数值
    String,    ///< 字符串
    Array,     ///< 数组
    Object     ///< 对象
};

/// @brief JSON 值类
/// @details 提供 JSON 值的表示和操作
/// @ingroup ParseFormat
class AST_UTIL_API JsonValue
{
public:
    /// @brief 默认构造函数，创建 null 值
    JsonValue();
    
    /// @brief 从布尔值构造
    /// @param value 布尔值
    explicit JsonValue(bool value);
    
    /// @brief 从整数构造
    /// @param value 整数值
    explicit JsonValue(int value);
    
    /// @brief 从双精度浮点数构造
    /// @param value 双精度浮点数值
    explicit JsonValue(double value);
    
    /// @brief 从字符串构造
    /// @param value 字符串值
    explicit JsonValue(const char* value);
    
    /// @brief 从字符串构造
    /// @param value 字符串值
    explicit JsonValue(const std::string& value);
    
    /// @brief 从字符串视图构造
    /// @param value 字符串视图
    explicit JsonValue(StringView value);
    
    /// @brief 从数组构造
    /// @param values JSON 值数组
    explicit JsonValue(const std::vector<JsonValue>& values);
    
    /// @brief 从对象构造
    /// @param values JSON 对象（键值对映射）
    explicit JsonValue(const std::map<std::string, JsonValue>& values);
    
    /// @brief 拷贝构造函数
    /// @param other 其他 JSON 值
    JsonValue(const JsonValue& other);
    
    /// @brief 移动构造函数
    /// @param other 其他 JSON 值
    JsonValue(JsonValue&& other) noexcept;
    
    /// @brief 拷贝赋值运算符
    /// @param other 其他 JSON 值
    /// @return 当前 JSON 值的引用
    JsonValue& operator=(const JsonValue& other);
    
    /// @brief 移动赋值运算符
    /// @param other 其他 JSON 值
    /// @return 当前 JSON 值的引用
    JsonValue& operator=(JsonValue&& other) noexcept;
    
    /// @brief 析构函数
    ~JsonValue();
    
    /// @brief 获取值类型
    /// @return JSON 值类型
    JsonValueType type() const;
    
    /// @brief 判断是否为 null 值
    /// @return 如果是 null 值返回 true，否则返回 false
    bool isNull() const;
    
    /// @brief 判断是否为布尔值
    /// @return 如果是布尔值返回 true，否则返回 false
    bool isBoolean() const;
    
    /// @brief 判断是否为数值
    /// @return 如果是数值返回 true，否则返回 false
    bool isNumber() const;
    
    /// @brief 判断是否为字符串
    /// @return 如果是字符串返回 true，否则返回 false
    bool isString() const;
    
    /// @brief 判断是否为数组
    /// @return 如果是数组返回 true，否则返回 false
    bool isArray() const;
    
    /// @brief 判断是否为对象
    /// @return 如果是对象返回 true，否则返回 false
    bool isObject() const;
    
    /// @brief 转换为布尔值（安全版本）
    /// @param defaultValue 默认值
    /// @return 布尔值，尝试转换为布尔值，否则返回默认值
    bool toBool(bool defaultValue = false) const;
    
    /// @brief 转换为整数值（安全版本）
    /// @param defaultValue 默认值
    /// @return 整数值，尝试转换为整数，否则返回默认值
    int toInt(int defaultValue = 0) const;
    
    /// @brief 转换为双精度浮点数值（安全版本）
    /// @param defaultValue 默认值
    /// @return 双精度浮点数值，尝试转换为浮点数，否则返回默认值
    double toDouble(double defaultValue = 0.0) const;
    
    /// @brief 转换为字符串值（安全版本）
    /// @param defaultValue 默认值
    /// @return 字符串值，尝试转换为字符串，否则返回默认值
    std::string toString(StringView defaultValue = StringView()) const;
    
    /// @brief 获取数组引用（安全版本）
    /// @param defaultValue 默认值
    /// @return 数组引用，如果类型不匹配返回默认值
    const std::vector<JsonValue>& getArray(const std::vector<JsonValue>& defaultValue = std::vector<JsonValue>()) const;
    
    /// @brief 获取对象引用（安全版本）
    /// @param defaultValue 默认值
    /// @return 对象引用，如果类型不匹配返回默认值
    const std::map<std::string, JsonValue>& getObject(const std::map<std::string, JsonValue>& defaultValue = std::map<std::string, JsonValue>()) const;
    
    
    /// @brief 转换为布尔值运算符
    /// @return 布尔值，null 值返回 false，数值非零返回 true，字符串非空返回 true
    operator bool() const;
    
    /// @brief 转换为整数值运算符
    /// @return 整数值，null 值返回 0，布尔值 true 返回 1，false 返回 0，字符串转换为整数
    operator int() const;
    
    /// @brief 转换为双精度浮点数值运算符
    /// @return 双精度浮点数值，null 值返回 0.0，布尔值 true 返回 1.0，false 返回 0.0，字符串转换为浮点数
    operator double() const;
    
    /// @brief 转换为字符串值运算符
    /// @return 字符串值，null 值返回 "null"，布尔值返回 "true" 或 "false"，数值转换为字符串
    operator std::string() const;
    
    
    /// @brief 数组下标运算符
    /// @param index 数组索引
    /// @return 对应索引的 JSON 值引用，如果类型不匹配或索引越界返回空值
    JsonValue& operator[](size_t index);
    
    /// @brief 数组下标运算符（常量版本）
    /// @param index 数组索引
    /// @return 对应索引的 JSON 值常量引用，如果类型不匹配或索引越界返回空值
    const JsonValue& operator[](size_t index) const;
    
    /// @brief 对象下标运算符
    /// @param key 对象键
    /// @return 对应键的 JSON 值引用，如果类型不匹配返回空值
    JsonValue& operator[](const std::string& key);
    
    /// @brief 对象下标运算符
    /// @param key 对象键
    /// @return 对应键的 JSON 值引用，如果类型不匹配返回空值
    JsonValue& operator[](const char* key);
    
    /// @brief 设置 null 值
    void setNull();
    
    /// @brief 设置布尔值
    /// @param value 布尔值
    void setBoolean(bool value);
    
    /// @brief 设置整数值
    /// @param value 整数值
    void setInt(int value);
    
    /// @brief 设置双精度浮点数值
    /// @param value 双精度浮点数值
    void setDouble(double value);
    
    /// @brief 设置字符串值
    /// @param value 字符串值
    void setString(const std::string& value);
    
    /// @brief 设置字符串值
    /// @param value 字符串值
    void setString(const char* value);
    
    /// @brief 设置数组值
    /// @param values JSON 值数组
    void setArray(const std::vector<JsonValue>& values);
    
    /// @brief 设置对象值
    /// @param values JSON 对象（键值对映射）
    void setObject(const std::map<std::string, JsonValue>& values);
    
    /// @brief 清空值
    void clear();
    
    /// @brief 转换为 JSON 字符串表示
    /// @return JSON 值的字符串表示
    std::string toJsonString() const;
    
    /// @brief 转换为格式化的 JSON 字符串表示
    /// @param indent 缩进空格数
    /// @return 格式化的 JSON 字符串
    std::string toFormattedString(int indent = 2) const;
    
private:
    JsonValueType type_;  ///< 值类型
    
    union
    {
        bool boolean_;                          ///< 布尔值
        double number_;                         ///< 数值
        std::string* string_;                   ///< 字符串值
        std::vector<JsonValue>* array_;         ///< 数组值
        std::map<std::string, JsonValue>* object_; ///< 对象值
    } value_;  ///< 值存储
    
    /// @brief 静态空值实例
    static JsonValue& NullValue();
    
    /// @brief 释放资源
    void destroy();
    
    /// @brief 格式化字符串辅助函数
    /// @param indent 当前缩进级别
    /// @param indentSize 缩进空格数
    /// @return 格式化的字符串
    std::string formatString(int indent, int indentSize) const;
};

AST_NAMESPACE_END