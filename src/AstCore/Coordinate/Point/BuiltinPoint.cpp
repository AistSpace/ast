///
/// @file      BuiltinPoint.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-11
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

#include "BuiltinPoint.hpp"
#include "AstCore/PlanetPoint.hpp"
#include "AstCore/BuiltinFrame.hpp"

AST_NAMESPACE_BEGIN

_AST_IMPL_POINT(Mercury,        ICRF)
_AST_IMPL_POINT(Venus,          ICRF)
_AST_IMPL_POINT(Earth,          ICRF)
_AST_IMPL_POINT(Mars,           ICRF)
_AST_IMPL_POINT(Jupiter,        ICRF)
_AST_IMPL_POINT(Saturn,         ICRF)
_AST_IMPL_POINT(Uranus,         ICRF)
_AST_IMPL_POINT(Neptune,        ICRF)
_AST_IMPL_POINT(Pluto,          ICRF)
_AST_IMPL_POINT(Moon,           ICRF)
_AST_IMPL_POINT(Sun,            ICRF)
_AST_IMPL_POINT(EMBarycenter,   ICRF)

AST_NAMESPACE_END
