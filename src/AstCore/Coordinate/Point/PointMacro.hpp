///
/// @file      PointMacro.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/BuiltinFrame.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Coordinate
    @{
*/

#define _AST_DECL_POINT(NAME) \
    class AST_CORE_API Point##NAME : public Point \
    { \
    public: \
        Point##NAME() = default; \
        ~Point##NAME() override= default; \
        static Point##NAME* Instance();\
        Frame* getFrame() const override;\
        err_t getPos(const TimePoint &tp, Vector3d &pos) const override;\
        err_t getPosVel(const TimePoint &tp, Vector3d &pos, Vector3d &vel) const override;\
    };\
    A_ALWAYS_INLINE Point* aPoint##NAME()\
    {\
        return Point##NAME::Instance();\
    }\


#define _AST_IMPL_POINT(NAME, FRAME)\
    Point##NAME* Point##NAME::Instance()\
    {\
        static SharedPtr<Point##NAME> instance(new Point##NAME());\
        return instance.get();\
    }\
    Frame* Point##NAME::getFrame() const\
    {\
        return Frame##FRAME::Instance();\
    }\
    err_t Point##NAME::getPos(const TimePoint &tp, Vector3d &pos) const\
    {\
        return a##NAME##PosIn##FRAME(tp, pos);\
    }\
    err_t Point##NAME::getPosVel(const TimePoint &tp, Vector3d &pos, Vector3d &vel) const\
    {\
        return a##NAME##PosVelIn##FRAME(tp, pos, vel);\
    }\


/*! @} */

AST_NAMESPACE_END

