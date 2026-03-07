///
/// @file      BaseParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-07
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

#include "BaseParser.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/IO.hpp"

AST_NAMESPACE_BEGIN

BaseParser::~BaseParser()
{
    close();
}

BaseParser::BaseParser(StringView filepath)
{
    open(filepath);
}

void BaseParser::open(StringView filepath)
{
    if(isOpen())
    {
        close();
    }
    file_ = ast_fopen(filepath.data(), "rb");  // 注意：这里使用二进制模式读取，避免文本模式下的ftell和fseek行为不一致
}

void BaseParser::close()
{
    if (file_ != stdout && file_ != nullptr && file_ != stderr)
    {
        if(!fileBorrowed_)
        {
            fclose(file_);
        }
    }
    file_ = nullptr;
}

void BaseParser::seek(std::streamoff pos, std::ios::seekdir dir)
{
    if (file_ != nullptr)
    {
        fseek(file_, (long)pos, (int)dir);
    }
    static_assert(std::ios::beg == SEEK_SET, "value not correct");
    static_assert(std::ios::cur == SEEK_CUR, "value not correct");
    static_assert(std::ios::end == SEEK_END, "value not correct");
}

std::streamoff BaseParser::tell()
{
    return std::streamoff(ftell(file_));
}

int BaseParser::getLineNumber()
{
    return aCurrentLineNumber(file_);
}

std::string BaseParser::getFilePath() const
{
    return aGetFilePath(file_);
}

void BaseParser::setBorrowedFile(FILE *file)
{
    if(file_ == file)
        return;
    this->close();
    file_ = file;
    fileBorrowed_ = true;
}

void BaseParser::setOwnedFile(FILE *file)
{
    if(file_ == file)
        return;
    this->close();
    file_ = file;
    fileBorrowed_ = false;
}


AST_NAMESPACE_END
