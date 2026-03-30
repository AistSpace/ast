///
/// @file      SerdeAPI.hpp
/// @brief     序列化模块的API头文件
/// @author    axel
/// @date      2026-03-30
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
#include "AstSerde.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstSerde
    @{
*/

/// @brief 将对象配置转换为 C++ 代码
/// @details 将对象配置转换为 C++ 代码，用于序列化和反序列化
/// @param object 对象指针
/// @param cppcode 输出的 C++ 代码
/// @return 错误码
AST_SERDE_CAPI errc_t aObjectToCppCode(Object* object, std::string& cppcode);

/// @brief 将对象序列化为指定格式
/// @details 将对象序列化为JSON、XML、C++、Java或Python格式
/// @param object 对象指针
/// @param format 序列化格式
/// @param output 输出字符串
/// @return 错误码
AST_SERDE_CAPI errc_t aObjectSerialize(Object* object, ESerializationFormat format, std::string& output);

/// @brief 从指定格式反序列化对象
/// @details 从JSON、XML、C++、Java或Python格式反序列化对象
/// @param input 输入字符串
/// @param format 序列化格式
/// @param object 对象指针
/// @return 错误码
AST_SERDE_CAPI errc_t aObjectDeserialize(const std::string& input, ESerializationFormat format, Object* object);

/*! @} */

AST_NAMESPACE_END