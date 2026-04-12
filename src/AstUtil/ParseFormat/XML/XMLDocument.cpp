///
/// @file      XMLDocument.cpp
/// @brief     XML文档类实现
/// @details   表示整个XML文档，包含根节点和文档级操作
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

#include "XMLDocument.hpp"
#include "XMLParser.hpp"

AST_NAMESPACE_BEGIN

XMLDocument::XMLDocument()
    : root_()
{
}

XMLDocument::~XMLDocument()
{
}

errc_t XMLDocument::load(StringView filepath)
{
    return root_.load(filepath);
}

errc_t XMLDocument::save(StringView filepath)
{
    return root_.save(filepath);
}


XMLNode* XMLDocument::getRoot() const
{
    return const_cast<XMLNode*>(&root_);
}

void XMLDocument::clear()
{
    root_.clear();
}

AST_NAMESPACE_END