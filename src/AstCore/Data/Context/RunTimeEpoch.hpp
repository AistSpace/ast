///
/// @file      RunTimeEpoch.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-16
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Data
    @{
*/

/// @brief 将时间点转换为相对参考历元的秒数
/// @param tp 时间点
/// @return 参考历元秒数
AST_CORE_CAPI double aTimePointToEpochSecond(const TimePoint& tp);

/// @brief 将相对参考历元的秒数转换为时间点
/// @param epochSecond 参考历元秒数
/// @param tp 时间点
AST_CORE_CAPI void aEpochSecondToTimePoint(double epochSecond, TimePoint& tp);

/// @brief 设置参考历元
/// @param tp 时间点
AST_CORE_CAPI void aSetEpoch(const TimePoint& tp);

/// @brief 获取参考历元
/// @return 参考历元
AST_CORE_CAPI void aGetEpoch(TimePoint& tp);



/*! @} */

AST_NAMESPACE_END
