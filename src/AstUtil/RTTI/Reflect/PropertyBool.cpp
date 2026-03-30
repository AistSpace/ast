///
/// @file      PropertyBool.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-09
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

#include "PropertyBool.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "AstUtil/PropertyVisitor.hpp"

AST_NAMESPACE_BEGIN


errc_t PropertyBool::getValueBool(const void* container, bool& value) 
{
    return getValue(container, &value);
}
errc_t PropertyBool::setValueBool(void* container, bool value) 
{
    return setValue(container, &value);
}
errc_t PropertyBool::getValueInt(const void* container, int& value) 
{
    bool b;
    errc_t ret = getValue(container, &b);
    if (ret != eNoError)
    {
        return ret;
    }
    value = b ? 1 : 0;
    return 0;
}
errc_t PropertyBool::setValueInt(void* container, int value) 
{
    bool b = value != 0;
    return setValue(container, &b);
}
errc_t PropertyBool::getValueString(const void* container, std::string& value)
{
    bool b;
    errc_t ret = getValue(container, &b);
    if (ret != eNoError)
    {
        return ret;
    }
    aFormatBool(b, value);
    return 0;
}
errc_t PropertyBool::setValueString(void* container, StringView value)
{
    bool b;
    errc_t ret = aParseBool(value, b);
    if (ret != eNoError)
    {
        return ret;
    }
    return setValue(container, &b);
}

errc_t PropertyBool::getValueDouble(const void* container, double& value)
{
    bool b;
    errc_t ret = getValue(container, &b);
    if (ret != eNoError)
    {
        return ret;
    }
    value = b ? 1.0 : 0.0;
    return 0;
}

errc_t PropertyBool::setValueDouble(void* container, double value)
{
    bool b = value != 0.0;
    return setValue(container, &b);
}

errc_t PropertyBool::accept(PropertyVisitor& visitor, const void* container)
{
    return visitor.visit(*this, container);
}

AST_NAMESPACE_END