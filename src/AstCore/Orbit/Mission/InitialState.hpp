///
/// @file      InitialState.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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
#include "MissionCommand.hpp"
#include "AstCore/SpacecraftState.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 初始状态任务
/// @details 初始状态任务负责设置初始状态参数，确保任务序列的执行从正确的状态开始
class AST_CORE_API InitialState: public MissionCommand
{
public:
    InitialState() = default;
    ~InitialState() = default;
public:
    errc_t execute() override;
    
    /// @brief 获取初始状态参数
    const SpacecraftState& getInitialState() const{return initialState_;}
    SpacecraftState& getInitialState(){return initialState_;}
private:
    SpacecraftState initialState_;      ///< 初始状态参数
};

/*! @} */

AST_NAMESPACE_END
