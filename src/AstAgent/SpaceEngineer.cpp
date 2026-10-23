///
/// @file      SpaceEngineer.cpp
/// @brief     航天工程师智能体
/// @details   定义面向航天任务设计与分析的默认智能体配置。
/// @author    axel
/// @date      2026-10-22
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

#include "SpaceEngineer.hpp"
#include "AstAgent/AgentInit.hpp"
#include "AstUtil/JsonValue.hpp"

AST_NAMESPACE_BEGIN

AssistantAgent aSpaceEngineerAgent()
{
    AssistantAgent agent;
    agent.setSystemPrompt(aAgentSystemPrompt());
    aInitAgentTools(agent.tools());

    // 是否禁用推理
    const bool disableThinking = true;

    if(disableThinking)
    {
        auto extraBody = R"(
            {
                "thinking":{
                    "type":"disabled"
                }
            }
        )"_json;
        agent.config().setExtraBody(extraBody);
    }
    return agent;
}

AssistantAgent* aNewSpaceEngineerAgent()
{
    return new AssistantAgent(aSpaceEngineerAgent());
}

AST_NAMESPACE_END
