///
/// @file      PropertyTimePoint.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-29
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

#include "PropertyTimePoint.hpp"
#include "AstUtil/ParseFormat.hpp"

AST_NAMESPACE_BEGIN


err_t PropertyTimePoint::getValueBool(const void* container, bool& value)
{
    TimePoint time;
    err_t ret = this->getValue(container, &time);
    if (ret != eNoError)
    {
        return ret;
    }
    // TimePoint to bool conversion - always return true for non-zero time
    value = true;
    return eNoError;
}

err_t PropertyTimePoint::setValueBool(void* container, bool value) 
{
    A_UNUSED(container);
    A_UNUSED(value);
    return eErrorInvalidParam;
}

err_t PropertyTimePoint::getValueInt(const void* container, int& value) 
{
    double d;
    err_t ret = getValueDouble(container, d);
    if (ret != eNoError)
    {
        return ret;
    }
    value = static_cast<int>(d);
    return eNoError;
}

err_t PropertyTimePoint::setValueInt(void* container, int value) 
{
    return setValueDouble(container, static_cast<double>(value));
}

err_t PropertyTimePoint::getValueString(const void* container, std::string& value)
{
    TimePoint time;
    err_t ret = this->getValue(container, &time);
    if (ret != eNoError)
    {
        return ret;
    }
    return aTimePointFormat(time, value);
}

err_t PropertyTimePoint::setValueString(void* container, StringView value)
{
    TimePoint time;
    err_t ret = aTimePointParse(value, time);
    if (ret != eNoError)
    {
        return ret;
    }
    return this->setValue(container, &time);
}

err_t PropertyTimePoint::getValueDouble(const void* container, double& value)
{
    TimePoint time;
    err_t ret = this->getValue(container, &time);
    if (ret != eNoError)
    {
        return ret;
    }
    value = aTimePointToEpochSecond(time);
    return eNoError;
}

err_t PropertyTimePoint::setValueDouble(void* container, double value)
{
    TimePoint time;
    aEpochSecondToTimePoint(value, time);
    return this->setValue(container, &time);
}

AST_NAMESPACE_END