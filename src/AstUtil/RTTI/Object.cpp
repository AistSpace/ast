/// @file      Object.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      19.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
 
#include "Object.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Class.hpp"            // for Class
#include "AstUtil/Property.hpp"         // for Property
#include "AstUtil/Attribute.hpp"        // for Attribute
#include "AstUtil/UiOperator.hpp"
#include "AstUtil/ObjectManager.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN
 

static_assert(sizeof(Object) == sizeof(void*) * 1 + sizeof(uint32_t) * 4, "size not correct");      // 检查 Object 类的大小是否正确

Class Object::staticType;

Object::Object(Object *parentScope)
    : Object{}
{
    setParentScope(parentScope);
}

Object::Object(std::nullptr_t)
    : Object{}
{
}

Class *Object::getType() const
{
    return &staticType;
}

const std::string empty;

const std::string &Object::getName() const
{
    return empty;
}

errc_t Object::openEditDialog()
{
    return aUiEditObject(this);
}

Attribute Object::attr(StringView path)
{
    return Attribute(this, this->getProperty(path));
}

errc_t Object::getAttrBool(StringView path, bool &value) const
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->getValueBool(this, value);
}

errc_t Object::getAttrInt(StringView path, int &value) const
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->getValueInt(this, value);
}

errc_t Object::getAttrDouble(StringView path, double &value) const
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->getValueDouble(this, value);
}

errc_t Object::getAttrString(StringView path, std::string &value) const
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->getValueString(this, value);
}

double Object::getAttrDouble(StringView path) const
{
    double value = 0.0;
    getAttrDouble(path, value);
    return value;
}

int Object::getAttrInt(StringView path) const
{
    int value = 0;
    getAttrInt(path, value);
    return value;
}

bool Object::getAttrBool(StringView path) const
{
    bool value = false;
    getAttrBool(path, value);
    return value;
}

std::string Object::getAttrString(StringView path) const
{
    std::string value;
    getAttrString(path, value);
    return value;
}

errc_t Object::setAttrBool(StringView path, bool value)
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->setValueBool(this, value);
}

errc_t Object::setAttrInt(StringView path, int value)
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->setValueInt(this, value);
}

errc_t Object::setAttrDouble(StringView path, double value)
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->setValueDouble(this, value);
}

errc_t Object::setAttrString(StringView path, StringView value)
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->setValueString(this, value);
}

Property *Object::getProperty(StringView fieldName) const
{
    auto type = getType();
    while(type)
    {
        auto prop = type->getProperty(fieldName);
        if(prop)
            return prop;
        type = type->getParent();
    }
    return nullptr;
}

uint32_t Object::getID() const
{
    if(index_ == static_cast<uint32_t>(INVALID_ID))
    {
        // @todo 从对象管理器获取对象ID
        return aAddObject(const_cast<Object*>(this));
    }
    return index_;
}

errc_t Object::setParentScope(Object *parentScope)
{
    return aSetParentScope(this, parentScope);
}

Object *Object::getParentScope() const
{
    return aGetParentScope(const_cast<Object*>(this));
}

Object::~Object()
{
    if(index_ != static_cast<uint32_t>(INVALID_ID))
    {
        errc_t rc = ObjectManager::CurrentInstance().removeNode(index_);
        if(rc != eNoError)
        {
            aError("failed to remove object from object manager");
        }
    }
}

AST_NAMESPACE_END