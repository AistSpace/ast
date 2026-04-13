///
/// @file      XMLSax.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-11
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
#include "AstUtil/BKVItemView.hpp"
#include <vector>


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief XML SAX解析器的回调接口
class AST_UTIL_API XMLSax {
public:
    virtual ~XMLSax() = default;
    using AttributeList = std::vector<BKVItemView>;
    /// @brief 文档开始回调
    virtual void startDocument() = 0;
    /// @brief 文档结束回调
    virtual void endDocument() = 0;
    /// @brief 元素开始回调
    virtual void startElement(StringView name, const AttributeList& attributes) = 0;
    /// @brief 元素结束回调
    virtual void endElement(StringView name) = 0;
    /// @brief 文本内容回调
    virtual void characters(StringView text) = 0;
    /// @brief 注释回调
    virtual void comment(StringView text){};
    /// @brief 错误回调
    /// @param msg 错误消息
    virtual void error(StringView msg){};
};



/*! @} */

AST_NAMESPACE_END