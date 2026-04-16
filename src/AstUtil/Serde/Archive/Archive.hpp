///
/// @file      Archive.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-31
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "AstUtil/Attribute.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 名值对
/// @tparam T 值类型
template<typename T>
class NameValuePair
{
public:
    const char* name() const { return name_; }
    T& value() const { return value_; }

    const char* name_;
    T& value_;
};

/// @brief 创建名值对
/// @param name 名称
/// @param val 值
/// @return NameValuePair<T> 名值对
template<typename T>
NameValuePair<T> make_nvp(const char* name, T& val)
{
    return NameValuePair<T>{name, val};
}


/// @brief 归档基类，实现对象的序列化和反序列化等操作
class AST_UTIL_API Archive
{
public:
    Archive() = default;
    virtual ~Archive() = default;
    virtual Archive& operator()(bool& val, const char* name=nullptr) = 0;
    virtual Archive& operator()(int& val, const char* name=nullptr) = 0;
    virtual Archive& operator()(double& val, const char* name=nullptr) = 0;
    virtual Archive& operator()(std::string& val, const char* name=nullptr) = 0;
    virtual Archive& operator()(Attribute& attr) {return *this;};
public:
    A_ALWAYS_INLINE Archive& operator&(bool& val) { return operator()(val); }
    A_ALWAYS_INLINE Archive& operator&(int& val) { return operator()(val); }
    A_ALWAYS_INLINE Archive& operator&(double& val) { return operator()(val); }
    A_ALWAYS_INLINE Archive& operator&(std::string& val) { return operator()(val); }
    A_ALWAYS_INLINE Archive& operator&(Attribute& attr) { return operator()(attr); }

    A_ALWAYS_INLINE Archive& operator&(const NameValuePair<bool>& nvp) { return operator()(nvp.value(), nvp.name()); }
    A_ALWAYS_INLINE Archive& operator&(const NameValuePair<int>& nvp) { return operator()(nvp.value(), nvp.name()); }
    A_ALWAYS_INLINE Archive& operator&(const NameValuePair<double>& nvp) { return operator()(nvp.value(), nvp.name()); }
    A_ALWAYS_INLINE Archive& operator&(const NameValuePair<std::string>& nvp) { return operator()(nvp.value(), nvp.name()); }
};

/*! @} */

AST_NAMESPACE_END
