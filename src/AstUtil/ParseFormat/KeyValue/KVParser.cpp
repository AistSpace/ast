///
/// @file      KVParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-20
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

#include "KVParser.hpp"
#include "AstUtil/ValueView.hpp"

AST_NAMESPACE_BEGIN


KVParser::EToken KVParser::getNext(BKVItemView &item)
{
    static_assert(eEOF == EOF, "eEOF must be EOF");

    if(!file_)
        return eError;
    
    while(fgets(lineBuffer_.data(), (int)lineBuffer_.size(), file_)){
        StringView line(lineBuffer_.data());
        if(aIsHashCommentLine(line))
            continue;
        auto pos = line.find('=');
        if(pos == StringView::npos)
            continue;
        item.value() = aStripAsciiWhitespace(line.substr(pos + 1));
        auto key = line.substr(0, pos);
        if(key.ends_with('+'))
        {
            item.key() = aStripAsciiWhitespace(key.substr(0, key.size() - 1));
            return eAddEqual;
        }else{
            item.key() = aStripAsciiWhitespace(key);
            return eEqual;
        }
    }
    return eEOF;
}


AST_NAMESPACE_END

