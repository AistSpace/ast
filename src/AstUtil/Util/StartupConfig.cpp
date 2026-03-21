///
/// @file      StartupConfig.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-20
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "StartupConfig.hpp"
#include "AstUtil/KVParser.hpp"
#include "AstUtil/IO.hpp"

AST_NAMESPACE_BEGIN

err_t StartupConfig::load(StringView filepath)
{
    KVParser parser;
    parser.open(filepath);
    if(!parser.isOpen())
        return eErrorInvalidFile;
    this->configMap_.clear();
    BKVItemView item;
    while(parser.getNext(item) == eNoError)
    {
        std::string value = this->decodeConfig(item.value());
        ValueView existsValue;
        if(this->getConfig(item.key(), existsValue) == eNoError)
        {
            value = existsValue.toString() + "," + value;
        }
        this->configMap_[std::string(item.key())] = value;
    }
    return eNoError;
}

ValueView StartupConfig::getConfig(StringView key) const
{
    ValueView value;
    err_t rc = this->getConfig(key, value);
    A_UNUSED(rc);
    return value;
}

err_t StartupConfig::getConfig(StringView key, ValueView &value) const
{
    auto it = this->configMap_.find(std::string(key));
    if(it == this->configMap_.end())
        return eErrorNotFound;
    value = it->second;
    return eNoError;
}

bool StartupConfig::hasConfig(StringView key) const
{
    auto it = this->configMap_.find(std::string(key));
    return it != this->configMap_.end();
}

std::string StartupConfig::decodeConfig(StringView value)
{
    auto pos = value.find('/');
    if(pos != std::string::npos)
    {
        StringView prefix = value.substr(0, pos);
        StringView suffix = value.substr(pos + 1);
        if(prefix.ends_with("_PATH") || prefix.ends_with("_DIR") || prefix.ends_with("_FILE"))
        {
            prefix = this->getConfig(prefix).toStringView();
            if(suffix.size() == 0){
                return std::string(prefix);
            }else{
                if(suffix.starts_with('/') || prefix.ends_with('/')){
                    return std::string(prefix) + std::string(suffix);
                }else{
                    return std::string(prefix) + "/" + std::string(suffix);
                }
            }
        }
    }
    return std::string(value);
}

void StartupConfig::printConfig(FILE* file) const
{
    for(auto it = this->configMap_.begin(); it != this->configMap_.end(); ++it)
    {
        posix::fprintf(file, "%20s=%s\n", it->first.c_str(), it->second.value().c_str());
    }
}



AST_NAMESPACE_END


