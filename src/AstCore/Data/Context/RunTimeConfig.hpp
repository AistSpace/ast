///
/// @file      RunTimeConfig.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-21
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "AstUtil/ValueView.hpp"
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Data
    @{
*/

/// @brief 获取当前线程的数据上下文的配置值
/// @param key 配置键
/// @return 配置值
AST_CORE_API ValueView aGetConfigValue(StringView key);

/// @brief 获取当前线程的数据上下文的配置字符串向量
/// @param key 配置键
/// @return 配置字符串向量
AST_CORE_API std::vector<std::string> aGetConfigStringVector(StringView key);


/// @brief 获取当前线程的数据上下文的配置字符串视图向量
/// @param key 配置键
/// @return 配置字符串视图向量
AST_CORE_API std::vector<StringView> aGetConfigStringViewVector(StringView key);



/*! @} */

AST_NAMESPACE_END
