///
/// @file      PropertyQuantity.cpp
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

#include "PropertyQuantity.hpp"
#include "AstUtil/Dimension.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstUtil/QuantityParser.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN



PropertyQuantity::PropertyQuantity(FPropertyGet getter, FPropertySet setter, Dimension dimension)
    : PropertyDouble(getter, setter)
    , dimension_(dimension)
{
}

PropertyQuantity::PropertyQuantity(StringView name, StringView desc, FPropertyGet getter, FPropertySet setter, Dimension dimension)
    : PropertyDouble(name, desc, getter, setter)
    , dimension_(dimension)
{
}

err_t PropertyQuantity::getValueString(const void *container, std::string &value)
{
    /// @todo 这里需要一个通过量纲获取国际制单位的函数
    return PropertyDouble::getValueString(container, value);
}

err_t PropertyQuantity::setValueString(void *container, StringView value)
{
    Quantity quant;
    err_t rc = aQuantityParse(value, quant);
    if(rc){
        aError("failed to parse quantity string: %.*s", (int)value.size(), value.data());
        return rc;
    }
    // 检查量纲是否匹配
    if(quant.dimension() != dimension_)
        return eErrorInvalidParam;
    return PropertyDouble::setValueDouble(container, quant.getInternalValue());
}

AST_NAMESPACE_END


