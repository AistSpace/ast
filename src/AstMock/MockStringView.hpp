///
/// @file      MockStringView.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      2.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include <string>
#if _HAS_CXX17
#include <string_view>
#endif
 
AST_NAMESPACE_BEGIN
 
AST_MOCK_API int aMockSizeStringConstRef(const std::string& str);
AST_MOCK_API int aMockSizeStringValue(std::string str);
AST_MOCK_API int aMockSizeStringView(StringView str);
AST_MOCK_API int aMockSizeStringViewRef(const StringView& str);

AST_MOCK_API const std::string& aMockStringValue();


AST_MOCK_API int aMockSetStringConstRef(const std::string& str);
AST_MOCK_API int aMockSetStringValue(std::string str);
AST_MOCK_API int aMockSetStringView(StringView str);
AST_MOCK_API int aMockSetStringViewRef(const StringView& str);


AST_MOCK_API int aMockCallByStringConstRef();
AST_MOCK_API int aMockCallByStringValue();
AST_MOCK_API int aMockCallByStringView();
AST_MOCK_API int aMockCallByStringConstRef(const std::string& str);
AST_MOCK_API int aMockCallByStringValue(std::string str);
AST_MOCK_API int aMockCallByStringView(StringView str);
AST_MOCK_API int aMockCallByStringViewRef(const StringView& str);
AST_MOCK_API int aMockCallByCharPtr(const char* str, size_t size);






#if _HAS_CXX17
AST_MOCK_CAPI int aMockSetStringViewStd(StringView str);
#endif

AST_NAMESPACE_END
 
