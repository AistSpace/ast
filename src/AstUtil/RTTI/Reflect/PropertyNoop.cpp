///
/// @file      PropertyNoop.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-01
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

#include "PropertyNoop.hpp"

AST_NAMESPACE_BEGIN

PropertyNoop *PropertyNoop::Instance()
{
    static PropertyNoop instance;
    return &instance;
}

errc_t PropertyNoop::accept(PropertyVisitor &visitor, const void *container)
{
    return eErrorInvalidParam;
}

errc_t PropertyNoop::getValueDouble(const void *container, double &value)
{
    return eErrorInvalidParam;
}

errc_t PropertyNoop::setValueDouble(void *container, double value)
{
    return eErrorInvalidParam;
}

errc_t PropertyNoop::getValueInt(const void *container, int &value)
{
    return eErrorInvalidParam;
}

errc_t PropertyNoop::setValueInt(void *container, int value)
{
    return eErrorInvalidParam;
}

errc_t PropertyNoop::getValueBool(const void *container, bool &value)
{
    return eErrorInvalidParam;
}

errc_t PropertyNoop::setValueBool(void *container, bool value)
{
    return eErrorInvalidParam;
}

errc_t PropertyNoop::getValueString(const void *container, std::string &value)
{
    return eErrorInvalidParam;
}

errc_t PropertyNoop::setValueString(void *container, StringView value)
{
    return eErrorInvalidParam;
}

AST_NAMESPACE_END

