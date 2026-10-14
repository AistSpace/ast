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

errc_t PluginLoader::load(StringView filepath)
{
    if (filepath.empty())
        return eErrorNullInput;

    void* handle = aLoadLibrary(filepath);
    if (!handle)
        return eErrorInvalidFile;

    // 元数据：通过元数据实现在不实例化任何对象的前提下识别插件
    const char* meta = static_cast<const char*>(aGetProcAddress(handle, AST_PLUGIN_METADATA_NAME));
    if (!meta)
    {
        // 不是插件（或未导出元数据），直接卸掉
        aFreeLibrary(handle);
        return eErrorInvalidFile;
    }

    std::vector<PluginEntry> entries;
    if (parseMetadata(meta, entries) != eNoError)
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

    auto fn = reinterpret_cast<FPluginInstance>(aGetProcAddress(handle_, AST_PLUGIN_ENTRY_NAME));
    if (!fn)
        return nullptr;

    instance_ = fn();
    return instance_;
}

errc_t PluginLoader::parseMetadata(StringView jsonText, std::vector<PluginEntry>& out)
{
    JsonValue doc;
    if (doc.parseFromString(jsonText) != eNoError)
        return eErrorParse;

    if (!doc.isArray())
        return eErrorParse;

    for (const JsonValue& item : doc.getArray())
    {
        if (!item.isObject())
            continue;

        PluginEntry entry;
        entry.iid = item["iid"].toString();
        if (entry.iid.empty())
            continue;

        const JsonValue& keys = item["keys"];
        if (keys.isArray())
        {
            for (const JsonValue& key : keys.getArray())
            {
                std::string name = key.toString();
                if (!name.empty())
                    entry.keys.push_back(std::move(name));
            }
        }

        out.push_back(std::move(entry));
    }

    return out.empty() ? eErrorParse : eNoError;
}

AST_NAMESPACE_END
