///
/// @file      XMLDomSax.hpp
/// @brief     XML DOM SAX解析器
/// @author    axel
/// @date      2026-04-12
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
#include "XMLSax.hpp"
#include "XMLNode.hpp"
#include <stack>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief XML DOM SAX解析器
/// @details 结合了SAX解析和DOM树构建的解析器，通过SAX回调构建DOM树
class AST_UTIL_API XMLDomSax : public XMLSax {
public:
    explicit XMLDomSax(XMLNode& document);
    ~XMLDomSax() override;

    /// @brief 文档开始回调
    void startDocument() override;

    /// @brief 文档结束回调
    void endDocument() override;

    /// @brief 元素开始回调
    void startElement(StringView name, const AttributeList& attributes) override;

    /// @brief 元素结束回调
    void endElement(StringView name) override;

    /// @brief 文本内容回调
    void characters(StringView text) override;

    /// @brief 注释回调
    void comment(StringView text) override;

    /// @brief 错误回调
    void error(StringView msg) override;

   

private:
    A_DISABLE_COPY(XMLDomSax);
    XMLNode& root_;                      ///< 文档根节点
    std::stack<XMLNode*> nodeStack_;     ///< 节点栈，用于构建DOM树
};

/*! @} */

AST_NAMESPACE_END