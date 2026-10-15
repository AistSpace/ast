///
/// @file      AtmospherePlugin.cpp
/// @brief     大气模型插件接入点实现
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

#include "AtmospherePlugin.hpp"
#include "AtmosphereProvider.hpp"
#include "AstCore/DragForce.hpp"
#include "AstUtil/PluginRegistry.hpp"
#include "AstUtil/Logger.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

Atmosphere* aCreateAtmosphereFromPlugin(StringView name, const DragForce& config)
{
    IAtmosphereProvider* provider = aPluginRegistry().find<IAtmosphereProvider>(name);
    if (!provider)
        return nullptr;
    return provider->create(config);
}

void aGetPluginAtmosphereModelNames(std::vector<std::string>& names)
{
    aPluginRegistry().keys<IAtmosphereProvider>(names);
}

AST_NAMESPACE_END
