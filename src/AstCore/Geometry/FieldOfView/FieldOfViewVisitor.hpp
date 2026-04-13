///
/// @file      FieldOfViewVisitor.hpp
/// @brief     视场访问者接口
/// @details   定义视场访问者的接口，用于访问不同类型的视场
/// @author    axel
/// @date      2026-04-10
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

class FieldOfView;
class FOVSimpleCone;
class FOVConical;
class FOVHalfPower;
class FOVRectangular;
class FOVSAR;
class FOVCustom;

class FieldOfViewVisitor
{
public:
    virtual ~FieldOfViewVisitor() = default;
    virtual void visit(FieldOfView& fov) = 0;
    virtual void visit(FOVSimpleCone& fov) = 0;
    virtual void visit(FOVConical& fov) = 0;
    virtual void visit(FOVHalfPower& fov) = 0;
    virtual void visit(FOVRectangular& fov) = 0;
    virtual void visit(FOVSAR& fov) = 0;
    virtual void visit(FOVCustom& fov) = 0;
};

/*! @} */

AST_NAMESPACE_END