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

/// @brief 插件入口函数类型
typedef PluginInstance* (*FPluginInstance)();


// 插件动态库导出符号名称

#define AST_PLUGIN_IID_NAME         ast_plugin_iid
#define AST_PLUGIN_KEYS_NAME        ast_plugin_keys
#define AST_PLUGIN_INSTANCE_NAME    ast_plugin_instance

#define AST_PLUGIN_IID_NAMESTR       A_STRINGIZE(AST_PLUGIN_IID_NAME)
#define AST_PLUGIN_KEYS_NAMESTR      A_STRINGIZE(AST_PLUGIN_KEYS_NAME)
#define AST_PLUGIN_INSTANCE_NAMESTR  A_STRINGIZE(AST_PLUGIN_INSTANCE_NAME)

#define AST_PLUGIN_INSTANCE(Type)   extern "C" AST_PLUGIN_EXPORT PluginInstance* AST_PLUGIN_INSTANCE_NAME(){static Type instance; return &instance;}
#define AST_PLUGIN_IID(IFace)       extern "C" AST_PLUGIN_EXPORT const char* AST_PLUGIN_IID_NAME = aInterfaceId<IFace>();
#define AST_PLUGIN_KEYS(...)        extern "C" AST_PLUGIN_EXPORT const char* AST_PLUGIN_KEYS_NAME[] = {__VA_ARGS__, nullptr};

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
