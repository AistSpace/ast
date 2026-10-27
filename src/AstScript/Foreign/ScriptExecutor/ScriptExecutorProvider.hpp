///
/// @file      ScriptExecutorProvider.hpp
/// @brief     脚本执行器提供者接口
/// @details   插件根对象实现该接口，按需创建具体语言的脚本执行器
/// @author    axel
/// @date      2026-10-27
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
#include "ScriptExecutor.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Script
    @{
*/

/// @brief 脚本执行器提供者接口
/// @details 插件根对象同时继承 PluginInstance 与本接口，通过 create 函数创建独立实例
class AST_SCRIPT_API IScriptExecutorProvider
{
public:
    virtual ~IScriptExecutorProvider() = default;

    /// @brief 创建脚本执行器实例
    /// @return 脚本执行器实例；所有权归调用方，需用 delete 释放
    virtual ScriptExecutor* create() const = 0;
};

/// @brief 脚本执行器提供者接口ID
#define AST_IID_IScriptExecutorProvider "space.ast.AstScript.IScriptExecutorProvider"

AST_DECLARE_INTERFACE(IScriptExecutorProvider, AST_IID_IScriptExecutorProvider)

/*! @} */

AST_NAMESPACE_END
