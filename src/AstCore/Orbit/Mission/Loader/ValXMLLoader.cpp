///
/// @file      ValXMLLoader.cpp
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

#include "ValXMLLoader.hpp"
#include "AstScript/ValDict.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/XMLSax.hpp"
#include "AstUtil/XMLParser.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Escape.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstUtil/QuantityParser.hpp"
#include <stack>
#include <vector>

AST_NAMESPACE_BEGIN


class ValXMLSax: public XMLSax
{
public:
    void startDocument() override
    {

    }
    void endDocument() override
    {

    }
    void startElement(StringView element, const AttributeList& attrs) override
    {
        currentDepth_ ++;
        
        if(inSTKObjectScope_){
            if(element == "OBJECT")
            {
                ParseContext& context = getCurrentContext();
                context = {};
                context.name_ = attrs.get("Name").toString();
                context.class_ = attrs.get("Class").toString();
            }else{
                aError("unknown element '%.*s', expect 'OBJECT'", element.size(), element.data());
            }
        }else if(element == "STKOBJECT")
        {
            inSTKObjectScope_ = true;
        }else {
            aError("root element 'STKOBJECT' is not found");
        }

    }
    void endElement(StringView element) override
    {
        if(element == "STKOBJECT")
        {
            inSTKObjectScope_ = false;
            value_ = stack_[currentDepth_ + 1].value_;
        }
        else if(element == "OBJECT")
        {
            ParseContext& context = getCurrentContext();
            if(context.class_ == "string")
            {
                context.value_ = aNewValueString(aUnquote(context.text_));
            }
            else if(context.class_ == "link")
            {
                // pass, do nothing here
            }
            else if(context.class_ == "bool")
            {
                context.value_ = aNewValueBool(aParseBool(context.text_));
            }
            else if(context.class_ == "date")
            {
                std::string datestr = aUnquote(context.text_);
                context.value_ = aNewValueString(datestr);
            }
            else if(context.class_ == "double")
            {
                Quantity quantity;
                errc_t rc = aQuantityParse(context.text_, quantity);
                // 很奇怪，有些数量值会以""结尾，导致解析数量值时失败，例如： 2.2 ""
                if(rc){
                    double d;
                    rc = aParseDouble(context.text_, d);
                    if(!StringView(context.text_).ends_with("\"\"") || rc != eNoError)
                    {
                        aError("failed to parse quantity '%.*s'", context.text_.size(), context.text_.data());
                    }
                    context.value_ = aNewValueDouble(d);
                }else{
                    context.value_ = aNewValueQuantity(quantity);
                }
            }
            else if(!context.value_)
            {
                context.value_ = ValDict::New();
            }
            ParseContext& parentContext = getParentContext();
            if(parentContext.class_ == "link")
            {
                parentContext.value_ = context.value_;
            }
            else
            {
                ValDict* parentDict{nullptr};
                if(!parentContext.value_)
                {
                    parentDict = ValDict::New();
                    parentContext.value_ = parentDict;
                }else{
                    parentDict = parentContext.value_->toValDict();
                }
                if(parentDict)
                {
                    parentDict->insert(context.name_, context.value_.get());
                }else{
                    aError("parent element is not a object or container");
                }
            }
        }else
        {
            aError("unknown element '%.*s', expect 'OBJECT'", element.size(), element.data());
        }
        currentDepth_ --;
        
    }
    void characters(StringView text) override
    {
        ParseContext& context = getCurrentContext();
        context.text_ += std::string(text);
    }
    void comment(StringView text)override
    {
        // pass
    }
    void error(StringView msg)override
    {
        // pass
    }
    Value* getValue()
    {
        return value_.get();
    }
private:
    struct ParseContext{
        std::string name_;
        std::string class_;
        std::string text_;
        SharedPtr<Value> value_;
    };

    ParseContext& getCurrentContext()
    {
        if(currentDepth_ >= stack_.size())
        {
            stack_.resize(currentDepth_ + 1);
        }
        return stack_[currentDepth_];
    }
    ParseContext& getParentContext()
    {
        size_t parentDepth = currentDepth_ - 1;
        if(parentDepth >= stack_.size())
        {
            stack_.resize(parentDepth + 1);
        }
        return stack_[parentDepth];
    }
private:
    size_t currentDepth_{0};                ///< 当前解析深度，1表示根元素
    std::vector<ParseContext> stack_;       ///< 解析上下文栈
    SharedPtr<Value> value_;                ///< 当前解析到的值
    bool inSTKObjectScope_{false};          ///< 是否在STKOBJECT元素范围内
    bool currentIsLeaf_{false};             ///< 是否正在解析叶子元素的文本，用于标识link
};

errc_t aLoadValue(StringView filepath, SharedPtr<Value>& value)
{
    ValXMLSax sax;
    XMLParser parser(filepath);
    errc_t rc = parser.parse(sax);
    value = sax.getValue();
#if 1
    if(auto dict = value->toValDict()){
        std::string json = dict->toJsonString(2);
        printf("json: %s\n", json.c_str());
    }
#endif
    return rc;
}


AST_NAMESPACE_END

