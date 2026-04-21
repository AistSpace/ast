///
/// @file      JsonParser.hpp
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
#include "AstUtil/BaseParser.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class JsonValue;
class JsonSax;

/// @brief JSON 解析器
/// @details 用于解析 JSON 字符串，将其转换为 JSON 值对象。
class AST_UTIL_API JsonParser
{
public:
    errc_t parseFromString(StringView json, JsonValue& value);
    errc_t parseFromFile(StringView file, JsonValue& value);
    errc_t parseFromString(StringView json, JsonSax& sax);
    errc_t parseFromFile(StringView file, JsonSax& sax);
private:
    
};

/*! @} */

AST_NAMESPACE_END
