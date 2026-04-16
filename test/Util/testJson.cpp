///
/// @file      testJson.cpp
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

#include "AstUtil/JsonValue.hpp"
#include "AstTest/Test.h"

#ifdef AST_WITH_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif
#ifdef AST_WITH_JSONCPP
#include <json/json.h>
#endif

#ifdef AST_WITH_NLOHMANN_JSON

TEST(JsonText, nlohmann_json)
{
    try{
        nlohmann::json value = 25;
        printf("value: %s\n", value.dump().c_str());
        double dval = value.get<double>();
        printf("dval: %f\n", dval);
        int ival = value.get<int>();
        printf("ival: %d\n", ival);
        bool bval = value.get<bool>();
        printf("bval: %d\n", bval);
        std::string sval = value;
        printf("sval: %s\n", sval.c_str());
    }catch(const nlohmann::json::exception& e){
        printf("error: %s\n", e.what());
    }catch(...){
        printf("error: unknown exception\n");
    }
    try{
        nlohmann::json value = "12.5";
        printf("value: %s\n", value.dump().c_str());
        double dval = value.get<double>();
        printf("dval: %f\n", dval);
        int ival = value.get<int>();
        printf("ival: %d\n", ival);
        bool bval = value.get<bool>();
        printf("bval: %d\n", bval);
        std::string sval = value;
        printf("sval: %s\n", sval.c_str());
    }catch(const nlohmann::json::exception& e){
        printf("error: %s\n", e.what());
    }catch(...){
        printf("error: unknown exception\n");
       }
}

#endif

#ifdef AST_WITH_JSONCPP

TEST(JsonText, jsoncpp)
{
    {
        Json::Value value = 25;
        printf("value: %s\n", value.asString().c_str());
        double dval = value.asDouble();
        printf("dval: %f\n", dval);
        int ival = value.asInt();
        printf("ival: %d\n", ival);
        bool bval = value.asBool();
        printf("bval: %d\n", bval);
        std::string sval = value.asString();
        printf("sval: %s\n", sval.c_str());
    }
    try
    {
        Json::Value value = "12.5";
        printf("value: %s\n", value.asString().c_str());
        double dval = value.asDouble();
        printf("dval: %f\n", dval);
        int ival = value.asInt();
        printf("ival: %d\n", ival);
        bool bval = value.asBool();
        printf("bval: %d\n", bval);
        std::string sval = value.asString();
        printf("sval: %s\n", sval.c_str());
    }
    catch(const std::exception& e){
        printf("error: %s\n", e.what());
    }catch(...){
        printf("error: unknown exception\n");
    }
}

#endif


GTEST_MAIN()