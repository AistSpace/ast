///
/// @file      State.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Object.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/EventTime.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 航天器状态
class AST_CORE_API State: public Object
{
public:
    State() = default;
    ~State() override = default;

    void setStateEpoch(EventTime* stateEpoch);

    void setStateEpoch(const TimePoint& stateEpoch);

    err_t getStateEpoch(TimePoint& stateEpoch) const;

    SharedPtr<EventTime>& getStateEpochHandler(){ return stateEpoch_; }

    void setCoordEpoch(EventTime* coordEpoch);

    void setCoordEpoch(const TimePoint& coordEpoch);

    err_t getCoordEpoch(TimePoint& coordEpoch) const;

    SharedPtr<EventTime>& getCoordEpochHandler(){ return coordEpoch_; }

    bool  getUseCoordEpoch() const{ return useCoordEpoch_; }

    void  setUseCoordEpoch(bool useCoordEpoch){ useCoordEpoch_ = useCoordEpoch; }
protected:
    bool                    useCoordEpoch_;        ///< 是否使用坐标历元
    SharedPtr<Frame>        frame_;                ///< 参考坐标系
    SharedPtr<EventTime>    coordEpoch_;           ///< 坐标历元时间
    SharedPtr<EventTime>    stateEpoch_;           ///< 状态历元时间
};

/*! @} */

AST_NAMESPACE_END
