///
/// @file      SpiceKernelPool.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-08
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "SpiceKernelPool.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN


void SpiceKernelPool::setData(StringView name, const SpiceData &data)
{
    dataMap_[std::string(name)] = data;
}

void SpiceKernelPool::setDoubleData(StringView name, const std::vector<double> &data)
{
    return setData(name, SpiceData(data));
}

void SpiceKernelPool::setIntData(StringView name, const std::vector<int> &data)
{
    return setData(name, SpiceData(data));
}

void SpiceKernelPool::setCharData(StringView name, const std::vector<char> &data)
{
    return setData(name, SpiceData(data));
}

const SpiceData *SpiceKernelPool::getData(StringView name) const
{
    auto it = dataMap_.find(std::string(name));
    if (it == dataMap_.end())
        return nullptr;
    return &it->second;
}

const std::vector<double> *SpiceKernelPool::getDoubleData(StringView name) const
{
    auto data = getData(name);
    if (data == nullptr)
        return nullptr;
    return data->getDoubleData();
}

const std::vector<int> *SpiceKernelPool::getIntData(StringView name) const
{
    auto data = getData(name);
    if (data == nullptr)
        return nullptr;
    return data->getIntData();
}

const std::vector<char> *SpiceKernelPool::getCharData(StringView name) const
{
    auto data = getData(name);
    if (data == nullptr)
        return nullptr;
    return data->getCharData();
}

AST_NAMESPACE_END


