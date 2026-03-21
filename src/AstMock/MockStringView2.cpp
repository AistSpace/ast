///
/// @file      MockStringView2.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
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

#include "MockStringView.hpp"

AST_NAMESPACE_BEGIN


int aMockCallByStringConstRef()
{
    return aMockSizeStringConstRef(aMockStringValue());
}

int aMockCallByStringValue()
{
    return aMockSizeStringValue(aMockStringValue());
}

int aMockCallByStringView()
{
    return aMockSizeStringView(aMockStringValue());
}

int aMockCallByStringConstRef(const std::string &str)
{
    return aMockSizeStringConstRef(str);
}

int aMockCallByStringValue(std::string str)
{
    return aMockSizeStringValue(str);
}

int aMockCallByStringView(StringView str)
{
    return aMockSizeStringView(str);
}

int aMockCallByStringViewRef(const StringView &str)
{
    return aMockSizeStringViewRef(str);
}

int aMockCallByCharPtr(const char *str, size_t size)
{
    return aMockSizeStringView(StringView(str, size));
}

AST_NAMESPACE_END
