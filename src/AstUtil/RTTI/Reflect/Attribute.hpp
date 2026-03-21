///
/// @file      Attribute.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-18
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Property.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


class Property;

/// @brief 对象的特定属性
class Attribute
{
public:
    Attribute(void* object, Property* property)
        : object_(object), property_(property)
    {
    }
    err_t getValueDouble(double& value) const
    {
        if(!property_ || !object_) return eErrorInvalidParam;
        return property_->getValueDouble(object_, value);
    }
    err_t setValueDouble(double value)
    {
        if(!property_ || !object_) return eErrorInvalidParam;
        return property_->setValueDouble(object_, value);
    }
    err_t getValueInt(int& value) const
    {
        if(!property_ || !object_) return eErrorInvalidParam;
        return property_->getValueInt(object_, value);
    }
    err_t setValueInt(int value)
    {
        if(!property_ || !object_) return eErrorInvalidParam;
        return property_->setValueInt(object_, value);
    }
    err_t getValueBool(bool& value) const
    {
        if(!property_ || !object_) return eErrorInvalidParam;
        return property_->getValueBool(object_, value);
    }
    err_t setValueBool(bool value)
    {
        if(!property_ || !object_) return eErrorInvalidParam;
        return property_->setValueBool(object_, value);
    }
    err_t getValueString(std::string& value) const
    {
        if(!property_ || !object_) return eErrorInvalidParam;
        return property_->getValueString(object_, value);
    }
    err_t setValueString(StringView value)
    {
        if(!property_ || !object_) return eErrorInvalidParam;
        return property_->setValueString(object_, value);
    }
public:
    double getValueDouble() const
    {
        double value = 0.0;
        getValueDouble(value);
        return value;
    }
    int getValueInt() const
    {
        int value = 0;
        getValueInt(value);
        return value;
    }
    bool getValueBool() const
    {
        bool value = false;
        getValueBool(value);
        return value;
    }
    std::string getValueString() const
    {
        std::string value;
        getValueString(value);
        return value;
    }
public:
    operator double() const
    {
        return getValueDouble();
    }
    operator int() const
    {
        return getValueInt();
    }
    operator bool() const
    {
        return getValueBool();
    }
    operator std::string() const
    {
        return getValueString();
    }
public:
    Attribute& operator=(double value)
    {
        setValueDouble(value);
        return *this;
    }
    Attribute& operator=(int value)
    {
        setValueInt(value);
        return *this;
    }
    // 避免对bool类型的隐式转换
    template<typename T, typename = typename std::enable_if<std::is_same<T, bool>::value>::type>
    Attribute& operator=(T value)
    {
        setValueBool(value);
        return *this;
    }
    Attribute& operator=(StringView value)
    {
        setValueString(value);
        return *this;
    }   
protected:
    void*     object_{nullptr};
    Property* property_{nullptr};
};

/*! @} */

AST_NAMESPACE_END
