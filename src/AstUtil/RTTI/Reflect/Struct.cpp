///
/// @file      Struct.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-10
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

#include "Struct.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Property.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/FileSystem.hpp"

AST_NAMESPACE_BEGIN

Struct::~Struct()
{
    for (auto property : properties_)
    {
        delete property;
    }
}

Property *Struct::addProperty(StringView name, Property *property)
{
    std::string strname = aAsciiStrToLower(name);
    if(property->name().empty()){
        property->setName(strname);
    }
    propertyMap_[strname] = property;
    properties_.push_back(property);
    return property;
}

Property *Struct::addProperty(Property *property)
{
    return addProperty(property->name(), property);
}

Property *Struct::getProperty(StringView name)
{
    auto it = propertyMap_.find(aAsciiStrToLower(name));
    if (it == propertyMap_.end())
    {
        return nullptr;
    }
    return it->second;
}

std::string Struct::getModuleName() const
{
    fs::path modulePath = aGetModulePathFromAddress(const_cast<Struct*>(this));
    std::string moduleName = modulePath.stem().string();
#ifdef _AST_ENABLE_DEBUG_SUFFIX
    auto it = moduleName.rfind(_AST_DEBUG_SUFFIX);
    if(it != std::string::npos){
        return moduleName.substr(0, it);
    }
#endif
    return moduleName;
}


AST_NAMESPACE_END
