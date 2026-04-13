///
/// @file      XMLDocument.hpp
/// @brief     XML文档类
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

#pragma once

#include "AstGlobal.h"
#include "XMLNode.hpp"
#include <memory>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief XML文档类
/// @details 表示整个XML文档，包含根节点和文档级操作
class AST_UTIL_API XMLDocument
{
public:
    XMLDocument();
    ~XMLDocument();

    /// @brief 加载XML文档
    /// @param filepath 文件路径
    /// @return 错误码
    errc_t load(StringView filepath);

    /// @brief 保存XML文档
    /// @param filepath 文件路径
    /// @return 错误码
    errc_t save(StringView filepath);

    /// @brief 获取根节点
    /// @return 根节点指针，如果不存在则返回nullptr
    XMLNode* getRoot() const;

    /// @brief 清除文档内容
    void clear();

private:
    A_DISABLE_COPY(XMLDocument);
    XMLNode root_; ///< 根节点
};

/*! @} */

AST_NAMESPACE_END