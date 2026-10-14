///
/// @file      PluginLoader.hpp
/// @brief     单个插件动态库的加载与元数据读取
/// @details   负责加载插件动态库、读出其元数据，并在需要时获取插件根对象
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
#include "PluginInterface.hpp"
#include <string>
#include <vector>
#include <mutex>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Plugin
    @{
*/

/// @brief 单个插件动态库加载器
/// @details 
/// 加载后读取元数据并解析为 PluginEntry 列表，按需获取插件的根对象
/// 插件动态库加载后将常驻到进程退出
class AST_UTIL_API PluginLoader
{
public:
    PluginLoader() = default;
    ~PluginLoader() = default;

    A_DISABLE_COPY(PluginLoader)

    /// @brief 加载动态库并解析其元数据
    /// @param filepath 动态库路径
    /// @return eNoError 表示这是一个合法插件；其他错误码表示加载失败或不是插件
    errc_t load(StringView filepath);

    /// @brief 是否为已验证的插件库
    bool isPlugin() const { return handle_ != nullptr; }

    /// @brief 已解析的 (接口, 索引键) 声明
    const std::vector<PluginEntry>& entries() const { return entries_; }

    /// @brief 动态库文件路径
    const std::string& filepath() const { return filepath_; }

    /// @brief 获取插件根对象
    /// @return 根对象指针；不是插件或入口缺失时返回 nullptr
    PluginInstance* instance();

private:
    /// @brief 由插件元数据解析出条目列表
    static errc_t parseMetadata(StringView jsonText, std::vector<PluginEntry>& out);

    void* handle_{nullptr};              ///< 动态库句柄
    std::string filepath_{};             ///< 动态库路径
    std::vector<PluginEntry> entries_{}; ///< 元数据条目
    PluginInstance* instance_{nullptr};  ///< 根对象缓存
};

/*! @} */

AST_NAMESPACE_END
