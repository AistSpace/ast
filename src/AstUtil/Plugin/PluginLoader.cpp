///
/// @file      PluginLoader.cpp
/// @brief     单个插件动态库的加载与元数据读取实现
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

#include "PluginLoader.hpp"
#include "AstUtil/LibraryLoader.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/Logger.hpp"
#include <utility>

AST_NAMESPACE_BEGIN


namespace{
    
errc_t parseMetadata(const char* iid, const char** keys, std::vector<PluginEntry>& out)
{
    if (!iid)
        return eErrorParse;
    PluginEntry entry;
    entry.iid = iid;
    if (keys)
    {
        // keys 是以 nullptr 结尾的字符串数组
        for (const char** p = keys; *p != nullptr; ++p)
        {
            entry.keys.push_back(*p);
        }
    }
    out.push_back(std::move(entry));
    return eNoError;
}
}

errc_t PluginLoader::load(StringView filepath)
{
    if (filepath.empty())
        return eErrorNullInput;

    void* handle = aLoadLibrary(filepath);
    if (!handle)
        return eErrorInvalidFile;

    // 元数据：通过元数据实现在不实例化任何对象的前提下识别插件

    // 注意符号的间接层数：
    // ast_plugin_iid 导出的是指针变量，符号地址是那个指针变量自身的地址，需要再解引用一次
    // ast_plugin_keys 导出的是数组，符号地址就是数组首地址，直接可用

    const char* const* iidAddr = static_cast<const char* const*>(aGetProcAddress(handle, AST_PLUGIN_IID_NAMESTR));
    const char** keys          = static_cast<const char**>(aGetProcAddress(handle, AST_PLUGIN_KEYS_NAMESTR));
    const char* iid            = iidAddr ? *iidAddr : nullptr;
    if (!iid)
    {
        // 不是插件（或未导出元数据），直接卸掉
        aFreeLibrary(handle);
        return eErrorInvalidFile;
    }

    std::vector<PluginEntry> entries;
    if (parseMetadata(iid, keys, entries) != eNoError)
    {
        aWarning(_("插件 '%.*s' 的元数据格式非法，已忽略"), filepath.size(), filepath.data());
        aFreeLibrary(handle);
        return eErrorParse;
    }

    // 成功，则声明其在进程退出时自动卸载
    handle_ = handle;
    aFreeLibraryOnExit(handle);
    filepath_.assign(filepath.data(), filepath.size());
    entries_ = std::move(entries);
    return eNoError;
}

PluginInstance* PluginLoader::instance()
{
    if (instance_)
        return instance_;

    if (!handle_)
        return nullptr;

    auto fn = reinterpret_cast<FPluginInstance>(aGetProcAddress(handle_, AST_PLUGIN_INSTANCE_NAMESTR));
    if (!fn)
        return nullptr;

    instance_ = fn();
    return instance_;
}


AST_NAMESPACE_END
