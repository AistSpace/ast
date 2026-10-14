///
/// @file      PluginInterface.hpp
/// @brief     通用插件接口定义
/// @details   定义插件根对象、接口 ID 声明机制、元数据导出宏与实例入口约定
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

/*!
    @addtogroup Plugin
    @{
*/

/// @brief 插件库导出符号声明
#define AST_PLUGIN_EXPORT A_DECL_EXPORT

/// @brief 插件根对象基类
/// @details 
/// 每个插件动态库通过入口函数导出唯一根对象
/// 根对象可同时实现多个接口，可使用 aPluginCast 转换为所需的接口类型
class AST_UTIL_API PluginInstance
{
public:
    virtual ~PluginInstance() = default;
};

/// @brief 插件元数据导出符号名
/// @details 
/// 插件通过 AST_PLUGIN_METADATA 宏导出该符号，内容是 JSON 文本
/// 在不实例化任何对象的前提下即可读取并建立索引
#define AST_PLUGIN_METADATA_NAME "ast_plugin_query_metadata"

/// @brief 插件实例入口函数名
/// @details 插件必须导出一个签名如下的 extern "C" 函数，符号名即此常量：
/// @code
///   extern "C" AST_PLUGIN_EXPORT ast::PluginInstance* ast_plugin_instance();
/// @endcode
#define AST_PLUGIN_ENTRY_NAME "ast_plugin_instance"

/// @brief 插件入口函数类型
typedef PluginInstance* (*FPluginInstance)();

/// @brief 导出插件元数据
/// @param json 元数据 JSON 字面量，schema 为：
/// @code
///   [ { "iid": "接口ID", "keys": ["索引键", ...] }, ... ]
/// @endcode
/// @note 数组的每个元素是一条独立的 (iid, keys) 声明，一个库可声明多条
#define AST_PLUGIN_METADATA(json) extern "C" AST_PLUGIN_EXPORT const char ast_plugin_query_metadata[] = json;


/// @brief 接口 ID 特化模板
template<class IFace>
struct InterfaceId;

/// @brief 声明一个插件接口及其唯一 ID
/// @param IFace 接口类型
/// @param IID   接口唯一标识字符串，插件元数据里的 "iid" 必须与此一致
#define AST_DECLARE_INTERFACE(IFace, IID)                    \
    template<> struct InterfaceId<IFace> {                   \
        static constexpr const char* value() { return IID; } \
    };

/// @brief 获取接口 ID
template<class IFace>
constexpr const char* aInterfaceId()
{
    return InterfaceId<IFace>::value();
}

/// @brief 把插件根对象转换为指定接口类型
/// @details 依赖 C++ RTTI，转换失败返回 nullptr。
template<class IFace>
IFace* aPluginCast(PluginInstance* instance)
{
    return dynamic_cast<IFace*>(instance);
}

/// @brief 插件元数据中的一条 (接口, 索引键) 声明
struct PluginEntry
{
    std::string iid{};                ///< 接口 ID
    std::vector<std::string> keys{};  ///< 该接口下的索引键
};

/*! @} */

AST_NAMESPACE_END
