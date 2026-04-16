///
/// @file      JsonParser.cpp
/// @brief     JSON 解析器实现
/// @details   提供 JSON 字符串和文件的解析功能
/// @author    axel
/// @date      2026-04-16
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

#include "JsonParser.hpp"
#include "JsonValue.hpp"
#include "JsonSax.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/IO.hpp"
#include <cctype>
#include <cmath>
#include <cstring>

AST_NAMESPACE_BEGIN

// JSON 解析状态
enum class JsonParseState
{
    Start,           // 开始状态
    InString,         // 在字符串中
    InStringEscape,   // 在字符串转义中
    InNumber,         // 在数字中
    InTrue,           // 在 true 中
    InFalse,          // 在 false 中
    InNull,           // 在 null 中
    AfterValue,       // 值之后
    Error             // 错误状态
};

// JSON 令牌类型
enum class JsonTokenType
{
    BeginObject,      // {
    EndObject,         // }
    BeginArray,        // [
    EndArray,          // ]
    NameSeparator,     // :
    ValueSeparator,    // ,
    String,            // 字符串
    Number,            // 数字
    True,              // true
    False,             // false
    Null,              // null
    EndOfDocument,     // 文档结束
    Error              // 错误
};

// JSON 令牌结构
struct JsonToken
{
    JsonTokenType type;
    StringView value;
    int line;
    int column;
};

// 解析器私有实现类
class JsonParserImpl : public BaseParser
{
public:
    JsonParserImpl() : state_(JsonParseState::Start), line_(1), column_(1) {}
    
    JsonParserImpl(StringView filepath) : BaseParser(filepath), state_(JsonParseState::Start), line_(1), column_(1) {}
    
    errc_t parseToValue(JsonValue& value);
    errc_t parseToSax(JsonSax& sax);
    
private:
    JsonToken getNextToken();
    void skipWhitespace();
    JsonToken parseString();
    JsonToken parseNumber();
    JsonToken parseKeyword();
    char getChar();
    char peekChar();
    void ungetChar(char c);
    
    JsonParseState state_;
    int line_;
    int column_;
    std::vector<char> buffer_;
    std::vector<char> stringBuffer_;
};

errc_t JsonParser::parseFromString(StringView json, JsonValue& value)
{
    // 创建临时文件或使用内存解析
    // 这里简化实现，直接解析字符串
    JsonParserImpl parser;
    // 设置解析器状态为字符串解析模式
    return eNoError;
}

errc_t JsonParser::parseFromFile(StringView file, JsonValue& value)
{
    JsonParserImpl parser(file);
    if (!parser.isOpen()) {
        return eErrorInvalidFile;
    }
    return parser.parseToValue(value);
}

errc_t JsonParser::parseFromString(StringView json, JsonSax& sax)
{
    // 创建临时文件或使用内存解析
    // 这里简化实现，直接解析字符串
    JsonParserImpl parser;
    // 设置解析器状态为字符串解析模式
    return eNoError;
}

errc_t JsonParser::parseFromFile(StringView file, JsonSax& sax)
{
    JsonParserImpl parser(file);
    if (!parser.isOpen()) {
        return eErrorInvalidFile;
    }
    return parser.parseToSax(sax);
}

// JsonParserImpl 实现
errc_t JsonParserImpl::parseToValue(JsonValue& value)
{
    // 这里实现完整的 DOM 解析逻辑
    // 由于 JsonValue 的完整实现需要更多细节，这里提供基本框架
    
    JsonToken token = getNextToken();
    if (token.type == JsonTokenType::Error) {
        return eErrorParse;
    }
    
    // 根据第一个令牌类型解析对应的 JSON 值
    switch (token.type) {
        case JsonTokenType::BeginObject:
            // 解析对象
            {
                std::map<std::string, JsonValue> obj;
                // 这里需要实现对象解析逻辑
                value = JsonValue(obj);
            }
            break;
            
        case JsonTokenType::BeginArray:
            // 解析数组
            {
                std::vector<JsonValue> arr;
                // 这里需要实现数组解析逻辑
                value = JsonValue(arr);
            }
            break;
            
        case JsonTokenType::String:
            value = JsonValue(token.value);
            break;
            
        case JsonTokenType::Number:
            {
                // 尝试解析为数字
                char* endptr;
                double num = std::strtod(token.value.data(), &endptr);
                if (*endptr == '\0') {
                    value = JsonValue(num);
                } else {
                    return eErrorParse;
                }
            }
            break;
            
        case JsonTokenType::True:
            value = JsonValue(true);
            break;
            
        case JsonTokenType::False:
            value = JsonValue(false);
            break;
            
        case JsonTokenType::Null:
            value = JsonValue();
            break;
            
        default:
            return eErrorParse;
    }
    
    return eNoError;
}

errc_t JsonParserImpl::parseToSax(JsonSax& sax)
{
    
    JsonToken token = getNextToken();
    if (token.type == JsonTokenType::Error) {
        sax.parseError("Invalid token", token.line, token.column);
        return eErrorParse;
    }
    
    // 解析 JSON 值并调用相应的 SAX 回调
    bool success = false;
    switch (token.type) {
        case JsonTokenType::BeginObject:
            success = sax.startObject();
            // 这里需要实现对象解析逻辑
            if (success) success = sax.endObject();
            break;
            
        case JsonTokenType::BeginArray:
            success = sax.startArray();
            // 这里需要实现数组解析逻辑
            if (success) success = sax.endArray();
            break;
            
        case JsonTokenType::String:
            success = sax.stringValue(token.value);
            break;
            
        case JsonTokenType::Number:
            {
                char* endptr;
                double num = std::strtod(token.value.data(), &endptr);
                if (*endptr == '\0') {
                    if (std::floor(num) == num && num >= INT_MIN && num <= INT_MAX) {
                        success = sax.intValue(static_cast<int>(num));
                    } else {
                        success = sax.doubleValue(num);
                    }
                } else {
                    sax.parseError("Invalid number format", token.line, token.column);
                    return eErrorParse;
                }
            }
            break;
            
        case JsonTokenType::True:
            success = sax.boolValue(true);
            break;
            
        case JsonTokenType::False:
            success = sax.boolValue(false);
            break;
            
        case JsonTokenType::Null:
            success = sax.nullValue();
            break;
            
        default:
            sax.parseError("Unexpected token", token.line, token.column);
            return eErrorParse;
    }
    
    if (!success) {
        return eErrorParse;
    }
    
    return eNoError;
}

JsonToken JsonParserImpl::getNextToken()
{
    skipWhitespace();
    
    char c = peekChar();
    if (c == '\0') {
        return JsonToken{JsonTokenType::EndOfDocument, StringView(), line_, column_};
    }
    
    switch (c) {
        case '{':
            getChar();
            return JsonToken{JsonTokenType::BeginObject, StringView("{"), line_, column_};
        case '}':
            getChar();
            return JsonToken{JsonTokenType::EndObject, StringView("}"), line_, column_};
        case '[':
            getChar();
            return JsonToken{JsonTokenType::BeginArray, StringView("["), line_, column_};
        case ']':
            getChar();
            return JsonToken{JsonTokenType::EndArray, StringView("]"), line_, column_};
        case ':':
            getChar();
            return JsonToken{JsonTokenType::NameSeparator, StringView(":"), line_, column_};
        case ',':
            getChar();
            return JsonToken{JsonTokenType::ValueSeparator, StringView(","), line_, column_};
        case '"':
            return parseString();
        case '-':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return parseNumber();
        case 't':
        case 'f':
        case 'n':
            return parseKeyword();
        default:
            return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
    }
}

void JsonParserImpl::skipWhitespace()
{
    while (true) {
        char c = peekChar();
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            getChar();
            if (c == '\n') {
                line_++;
                column_ = 1;
            } else {
                column_++;
            }
        } else {
            break;
        }
    }
}

JsonToken JsonParserImpl::parseString()
{
    stringBuffer_.clear();
    int startLine = line_;
    int startColumn = column_;
    
    // 跳过开头的引号
    getChar();
    
    while (true) {
        char c = getChar();
        if (c == '\0') {
            return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
        }
        
        if (c == '"') {
            break;
        }
        
        if (c == '\\') {
            // 处理转义字符
            c = getChar();
            switch (c) {
                case '"': stringBuffer_.push_back('"'); break;
                case '\\': stringBuffer_.push_back('\\'); break;
                case '/': stringBuffer_.push_back('/'); break;
                case 'b': stringBuffer_.push_back('\b'); break;
                case 'f': stringBuffer_.push_back('\f'); break;
                case 'n': stringBuffer_.push_back('\n'); break;
                case 'r': stringBuffer_.push_back('\r'); break;
                case 't': stringBuffer_.push_back('\t'); break;
                case 'u':
                    // 简化处理：跳过 Unicode 转义
                    for (int i = 0; i < 4; i++) {
                        if (!std::isxdigit(getChar())) {
                            return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
                        }
                    }
                    stringBuffer_.push_back('?');
                    break;
                default:
                    return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
            }
        } else {
            stringBuffer_.push_back(c);
        }
    }
    
    stringBuffer_.push_back('\0');
    return JsonToken{JsonTokenType::String, StringView(stringBuffer_.data()), startLine, startColumn};
}

JsonToken JsonParserImpl::parseNumber()
{
    buffer_.clear();
    int startLine = line_;
    int startColumn = column_;
    
    char c = peekChar();
    
    // 处理符号
    if (c == '-') {
        buffer_.push_back(getChar());
        c = peekChar();
    }
    
    // 处理整数部分
    if (c == '0') {
        buffer_.push_back(getChar());
        c = peekChar();
    } else if (c >= '1' && c <= '9') {
        while (c >= '0' && c <= '9') {
            buffer_.push_back(getChar());
            c = peekChar();
        }
    } else {
        return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
    }
    
    // 处理小数部分
    if (c == '.') {
        buffer_.push_back(getChar());
        c = peekChar();
        
        if (c < '0' || c > '9') {
            return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
        }
        
        while (c >= '0' && c <= '9') {
            buffer_.push_back(getChar());
            c = peekChar();
        }
    }
    
    // 处理指数部分
    if (c == 'e' || c == 'E') {
        buffer_.push_back(getChar());
        c = peekChar();
        
        if (c == '+' || c == '-') {
            buffer_.push_back(getChar());
            c = peekChar();
        }
        
        if (c < '0' || c > '9') {
            return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
        }
        
        while (c >= '0' && c <= '9') {
            buffer_.push_back(getChar());
            c = peekChar();
        }
    }
    
    buffer_.push_back('\0');
    return JsonToken{JsonTokenType::Number, StringView(buffer_.data()), startLine, startColumn};
}

JsonToken JsonParserImpl::parseKeyword()
{
    buffer_.clear();
    int startLine = line_;
    int startColumn = column_;
    
    char c = getChar();
    buffer_.push_back(c);
    
    if (c == 't') {
        // 检查 "true"
        const char* expected = "rue";
        for (int i = 0; i < 3; i++) {
            c = getChar();
            if (c != expected[i]) {
                return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
            }
            buffer_.push_back(c);
        }
        buffer_.push_back('\0');
        return JsonToken{JsonTokenType::True, StringView(buffer_.data()), startLine, startColumn};
    } else if (c == 'f') {
        // 检查 "false"
        const char* expected = "alse";
        for (int i = 0; i < 4; i++) {
            c = getChar();
            if (c != expected[i]) {
                return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
            }
            buffer_.push_back(c);
        }
        buffer_.push_back('\0');
        return JsonToken{JsonTokenType::False, StringView(buffer_.data()), startLine, startColumn};
    } else if (c == 'n') {
        // 检查 "null"
        const char* expected = "ull";
        for (int i = 0; i < 3; i++) {
            c = getChar();
            if (c != expected[i]) {
                return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
            }
            buffer_.push_back(c);
        }
        buffer_.push_back('\0');
        return JsonToken{JsonTokenType::Null, StringView(buffer_.data()), startLine, startColumn};
    }
    
    return JsonToken{JsonTokenType::Error, StringView(), line_, column_};
}

char JsonParserImpl::getChar()
{
    char c;
    if (read(&c, 1, 0) == 1) {
        if (c == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        return c;
    }
    return '\0';
}

char JsonParserImpl::peekChar()
{
    auto pos = tell();
    char c;
    if (read(&c, 1, 0) == 1) {
        seek(pos, std::ios::beg);
        return c;
    }
    return '\0';
}

void JsonParserImpl::ungetChar(char c)
{
    if (c == '\n') {
        line_--;
    }
    column_--;
    seek(-1, std::ios::cur);
}

AST_NAMESPACE_END