///
/// @file      TestTLE.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-17
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



/// @brief 往返自检：序列化后重新解析，逐字段比对
/// @details 用于保证写出格式与 TLE::FromLines 的解析保持一致。容差按各字段的
///          输出量化步长设定（角度 1e-4 度、偏心率 1e-7、平运动 1e-8 圈/天等）。
/// @return eNoError 一致；eErrorInvalidValue 不一致
AST_TEST_API errc_t aTLERoundTripCheck(const TLE& tle);


/// @brief 往返自检：解析后重新序列化，逐字段比对
/// @return eNoError 一致；eErrorInvalidValue 不一致
AST_TEST_API errc_t aTLELinesRoundTripCheck(const TLELines& tlelines);

/*! @} */

AST_NAMESPACE_END
