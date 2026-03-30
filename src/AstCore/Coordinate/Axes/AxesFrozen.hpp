///
/// @file      AxesFrozen.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-15
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
#include "AstCore/Axes.hpp"
#include "AstCore/EventTime.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AxesFrozen;
using PAxesFrozen = AxesFrozen*;
using HAxesFrozen = SharedPtr<AxesFrozen>;

/// @brief 冻结轴系
class AST_CORE_API AxesFrozen: public Axes
{
public:
    static PAxesFrozen New(Axes* axes, const TimePoint& epoch, Axes* referenceAxes);
    static HAxesFrozen MakeShared(Axes* axes, const TimePoint& epoch, Axes* referenceAxes);

    AxesFrozen() = default;
    AxesFrozen(Axes* axes, const TimePoint& epoch, Axes* referenceAxes);
    ~AxesFrozen() = default;
public:
    Axes* getParent() const override;
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;
PROPERTIES:
    const TimePoint& getEpoch() const { return epoch_; }
    void setEpoch(const TimePoint& epoch) { epoch_ = epoch; }
    Axes* getAxes() const { return axes_; }
    void setAxes(Axes* axes) { axes_ = axes; }
    Axes* getReferenceAxes() const { return referenceAxes_; }
    void setReferenceAxes(Axes* referenceAxes) { referenceAxes_ = referenceAxes; }
protected:
    SharedPtr<Axes> referenceAxes_;     ///< 参考轴系
    SharedPtr<Axes> axes_;              ///< 冻结轴系
    TimePoint       epoch_;             ///< 冻结时间
};

/*! @} */

AST_NAMESPACE_END
