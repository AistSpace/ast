///
/// @file      AtmospherePlugin.hpp
/// @brief     大气模型插件的接入点
/// @details   让第三方以独立动态库的形式提供大气模型
/// @author    axel
/// @date      2026-10-14
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
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

class Atmosphere;
class DragForce;

/*!
    @addtogroup Core
    @{
*/

/// @brief 按名字创建插件提供的大气模型
/// @param name 模型名字，来自插件元数据里的索引键
/// @param config 大气阻力模型配置
/// @return 模型实例；未找到或创建失败返回 nullptr。所有权归调用方
/// @note 插件需要先被发现：调用 aScanPlugins()
AST_CORE_API Atmosphere* aCreateAtmosphereFromPlugin(StringView name, const DragForce& config);

/// @brief 列出已发现插件提供的大气模型名字
AST_CORE_API void aGetPluginAtmosphereModelNames(std::vector<std::string>& names);

/*! @} */

AST_NAMESPACE_END
