///
/// @file      EnumOrName.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-15
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

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 枚举或名字
/// @details 既可赋值枚举值，也可赋值名字字符串
/// @code
///   EnumOrName<EAtmosphereModel> model;
///   model = EAtmosphereModel::eMSISE1990;    // 枚举
///   model = "Exponential";                   // 名字
/// @endcode
template<typename EnumType>
class EnumOrName
{
public:
    EnumOrName() = default;
    EnumOrName(EnumType value) : value_(value) {}
    EnumOrName(StringView name): name_(name) {}
    EnumOrName(const std::string& name) : name_(name) {}
    EnumOrName(const char* name) : name_(name) {}

    EnumType value() const { return value_; }
    const std::string& name() const { return name_; }

private:
    EnumType value_{};      ///< 值
    std::string name_{};    ///< 名字
};


/*! @} */

AST_NAMESPACE_END
