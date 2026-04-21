///
/// @file      ValDict.cpp
/// @brief     
/// @details   
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

#include "ValDict.hpp"
#include "ValString.hpp"
#include "ValBool.hpp"
#include "ValNull.hpp"
#include "ValArray.hpp"

AST_NAMESPACE_BEGIN

ValDict* ValDict::New()
{
    return new ValDict();
}

Value *ValDict::find(const std::string &name)
{
    auto it = map_.find(name);
    return it != map_.end() ? it->second.get() : nullptr;
}

void ValDict::insert(const std::string &name, Value *value)
{
    map_.insert({name, value});
}

std::string ValDict::toJsonString() const
{
    return toJsonString(0);
}

std::string ValDict::toJsonString(int indent) const
{
    return toJsonString(indent, 0);
}

std::string ValDict::getExpression(Object *context) const
{
    return "Dict()";
}

std::string ValDict::toJsonString(int indent, int currentIndent) const
{
    std::string result;
    bool first = true;
    
    // 添加缩进
    // if (indent > 0) {
    //     // result += "\n";
    //     // result.append(currentIndent, ' ');
    // }
    
    result += "{";
    
    if (indent > 0 && !map_.empty()) {
        result += "\n";
    }
    
    for (const auto& pair : map_) {
        if (!first) {
            result += ",";
            if (indent > 0) {
                result += "\n";
            }
        }
        first = false;
        
        // 添加缩进
        if (indent > 0) {
            result.append(currentIndent + indent, ' ');
        }
        
        // 转义键名
        result += "\"";
        for (char c : pair.first) {
            switch (c) {
                case '\\': result += "\\\\"; break;
                case '\"': result += "\\\""; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        char buf[7];
                        snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                        result += buf;
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        result += "\": ";
        
        // 处理值
        if (pair.second) {
            if (pair.second->getType() == &ValString::staticType) {
                // 字符串值需要转义
                auto strValue = static_cast<ValString*>(pair.second.get())->value();
                result += "\"";
                for (char c : strValue) {
                    switch (c) {
                        case '\\': result += "\\\\"; break;
                        case '\"': result += "\\\""; break;
                        case '\b': result += "\\b"; break;
                        case '\f': result += "\\f"; break;
                        case '\n': result += "\\n"; break;
                        case '\r': result += "\\r"; break;
                        case '\t': result += "\\t"; break;
                        default:
                            if (static_cast<unsigned char>(c) < 0x20) {
                                char buf[7];
                                snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                                result += buf;
                            } else {
                                result += c;
                            }
                            break;
                    }
                }
                result += "\"";
            } else if (pair.second->getType() == &ValBool::staticType) {
                // 布尔值
                auto boolValue = static_cast<ValBool*>(pair.second.get())->value();
                result += boolValue ? "true" : "false";
            } else if (pair.second->getType() == &ValNull::staticType) {
                // null 值
                result += "null";
            } else if (pair.second->getType() == &ValDict::staticType) {
                // 嵌套的 ValDict
                auto mapValue = static_cast<ValDict*>(pair.second.get());
                result += mapValue->toJsonString(indent, currentIndent + indent);
            } else if (pair.second->getType()->name().find("ValArray") != std::string::npos) {
                // 数组（暂时返回空数组）
                result += "[]";
            } else {
                // 其他数值类型使用 toString()
                result += pair.second->toString();
            }
        } else {
            result += "null";
        }
    }
    
    if (indent > 0 && !map_.empty()) {
        result += "\n";
        result.append(currentIndent, ' ');
    }
    
    result += "}";
    return result;
}

AST_NAMESPACE_END