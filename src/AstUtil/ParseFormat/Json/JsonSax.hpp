///
/// @file      JsonSax.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-16
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
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_UTIL_API JsonSax
{
public:
    virtual ~JsonSax() = default;
    
    /// @brief 开始解析对象
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool startObject() { return true; }
    
    /// @brief 结束解析对象
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool endObject() { return true; }
    
    /// @brief 开始解析数组
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool startArray() { return true; }
    
    /// @brief 结束解析数组
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool endArray() { return true; }
    
    /// @brief 解析对象键
    /// @param key 对象键
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool key(StringView key) { return true; }
    
    /// @brief 解析 null 值
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool nullValue() { return true; }
    
    /// @brief 解析布尔值
    /// @param value 布尔值
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool boolValue(bool value) { return true; }
    
    /// @brief 解析整数值
    /// @param value 整数值
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool intValue(int value) { return true; }
    
    /// @brief 解析双精度浮点数值
    /// @param value 双精度浮点数值
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool doubleValue(double value) { return true; }
    
    /// @brief 解析字符串值
    /// @param value 字符串值
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool stringValue(StringView value) { return true; }
    
    /// @brief 解析错误
    /// @param message 错误消息
    /// @param line 错误行号
    /// @param column 错误列号
    /// @return 如果继续解析返回 true，否则返回 false
    virtual bool parseError(StringView message, int line, int column) { return false; }
};

/*! @} */

AST_NAMESPACE_END