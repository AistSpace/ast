///
/// @file      AtmosphereProvider.hpp
/// @brief     大气模型提供者接口
/// @details   内置大气模型与插件大气模型都通过实现该接口接入统一的查找表。
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
#include "AstUtil/PluginInterface.hpp"

AST_NAMESPACE_BEGIN

class Atmosphere;
class DragForce;

/*!
    @addtogroup Core
    @{
*/

/// @brief 大气模型提供者接口
class AST_CORE_API IAtmosphereProvider
{
public:
    virtual ~IAtmosphereProvider() = default;

    /// @brief 创建模型实例
    /// @param config 大气阻力模型配置
    /// @return 模型实例；不支持或初始化失败时返回 nullptr。所有权归调用方
    virtual Atmosphere* create(const DragForce& config) const = 0;
};

/// @brief 大气模型提供者接口的唯一 ID（插件元数据里的 "iid" 必须是此值）
#define AST_IID_IAtmosphereProvider "space.ast.AstCore.IAtmosphereProvider"

AST_DECLARE_INTERFACE(IAtmosphereProvider, AST_IID_IAtmosphereProvider)

/*! @} */

AST_NAMESPACE_END
