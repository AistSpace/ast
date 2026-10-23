///
/// @file      ChatTools.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-27
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
#include "ChatTool.hpp"
#include <vector>
#include <memory>
#include <utility>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief AI工具集合
/// @details 所有AI工具的集合，用于管理工具的添加、删除和调用。
///          同名工具只保留第一个；工具可以在添加后再设置名称。
class AST_HARNESS_API ChatTools
{
public:
    ChatTools() = default;
    ~ChatTools() = default;
    ChatTools(ChatTools&&) = default;
    ChatTools& operator=(ChatTools&&) = default;
public:
    /// @brief 处理工具调用
    /// @param toolCall 工具调用参数
    /// @return 工具调用结果
    std::string handleToolCall(const JsonValue& toolCall);

    /// @brief 添加工具
    /// @param tool 工具指针
    /// @return 添加成功返回工具指针；同名工具已存在时返回nullptr
    /// @note 工具名称可以为空，添加后再通过 ChatTool::setName 设置
    ChatTool* addTool(std::unique_ptr<ChatTool>&& tool);

    /// @brief 添加函数形式的工具
    /// @param func 工具函数，签名为 std::string(const JsonValue&)
    /// @return 添加成功返回工具指针；同名工具已存在时返回nullptr
    template<typename Func>
    ChatTool* addTool(Func func)
    {
        return addTool(std::unique_ptr<ChatTool>(new ChatToolGeneric<Func>(std::move(func))));
    }

    /// @brief 清除所有工具
    void clearTools(){tools_.clear();}

    /// @brief 查找工具
    /// @param name 工具名称
    /// @return 工具指针，不存在返回nullptr
    ChatTool* findTool(StringView name) const;

    /// @brief 获取工具列表
    const std::vector<std::unique_ptr<ChatTool>>& tools() const {return tools_;}

    /// @brief 获取工具数量
    size_t size() const {return tools_.size();}

    /// @brief 将工具集合转换为JSON格式
    /// @return JSON值
    JsonValue toJson() const;

    /// @brief 检查工具集合是否为空
    bool empty() const {return tools_.empty();}
private:
    A_DISABLE_COPY(ChatTools);
    std::vector<std::unique_ptr<ChatTool>> tools_{};
};


/*! @} */

AST_NAMESPACE_END
