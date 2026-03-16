///
/// @file      FrameXXXXXX.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/Frame.hpp"
#include "AstCore/EventTime.hpp"
#include "AstCore/AxesFrozenAtEventTime.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/Point.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class FrameWithEpoch;
using PFrameWithEpoch = FrameWithEpoch*;
using HFrameWithEpoch = SharedPtr<FrameWithEpoch>;

/// @brief 带轴系历元的坐标系
class AST_CORE_API FrameWithEpoch: public Frame
{
public:
    static PFrameWithEpoch New(Point* origin, Axes* axes);
    static PFrameWithEpoch New(Point* origin, Axes* axes, EventTime* epoch);
    static HFrameWithEpoch MakeShared(Point* origin, Axes* axes);
    static HFrameWithEpoch MakeShared(Point* origin, Axes* axes, EventTime* epoch);

    FrameWithEpoch();
    FrameWithEpoch(Point* origin, Axes* axes);
    FrameWithEpoch(Point* origin, Axes* axes, EventTime* epoch);
    ~FrameWithEpoch() = default;
public:
    Axes* getAxes() const override;
    Point* getOrigin() const override;
public:
    bool getUseEpoch() const{ return useEpoch_; }
    void setUseEpoch(bool useEpoch){ useEpoch_ = useEpoch; }
    SharedPtr<EventTime>& getEpochHandle();
    EventTime* getEpoch() const;
    err_t getEpoch(TimePoint& epoch) const;
    void setEpoch(EventTime* epoch);
    void setSourceAxes(Axes* axes);
    Axes* getSourceAxes() const;
    Axes* getReferenceAxes() const;
    void setReferenceAxes(Axes* axes);
protected:
    SharedPtr<Point>                      origin_;               ///< 原点
    SharedPtr<AxesFrozenAtEventTime>      frozenAxes_;           ///< 冻结轴系
    SharedPtr<Axes>                       sourceAxes_;           ///< 轴系
    bool                                  useEpoch_{false};      ///< 是否使用轴系历元
};

/*! @} */

AST_NAMESPACE_END