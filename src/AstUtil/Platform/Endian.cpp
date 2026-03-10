///
/// @file      Endian.cpp
/// @brief     端序转换函数
/// @details   提供端序转换函数，用于处理不同端序的系统之间的数据交换
/// @author    axel
/// @date      2026-03-10
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

#include "Endian.hpp"

AST_NAMESPACE_BEGIN

EEndian aGetEndian()
{
    int test = 1;
    char* firstByte = reinterpret_cast<char*>(&test);
    return (*firstByte == 1) ? EEndian::eLittleEndian : EEndian::eBigEndian;
}

AST_NAMESPACE_END


