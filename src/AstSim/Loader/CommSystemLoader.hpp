///
/// @file      CommSystemLoader.hpp
/// @brief     通信系统加载器
/// @author    axel
/// @date      2026-04-07
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class CommSystem;

/// @brief 加载通信系统对象
/// @details 从文件加载通信系统对象
/// @param filepath 文件路径
/// @param commSystem 通信系统对象引用
/// @return 错误码
AST_SIM_CAPI errc_t aLoadCommSystem(StringView filepath, CommSystem& commSystem);

/*! @} */

AST_NAMESPACE_END