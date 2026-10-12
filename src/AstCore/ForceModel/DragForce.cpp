///
/// @file      DragForce.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-21
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

#include "DragForce.hpp"
#include "AstWeather/GeomagneticIndex.hpp"
#include "AstUtil/EnumDescriptor.hpp"

AST_NAMESPACE_BEGIN

double DragForce::ap() const
{
    return aKpToAp(kp_);
}


AST_ENUM_DESCRIPTOR(EAtmDensityModel, 
    AST_ENUM_VALUE(eNone,           "None"),
    AST_ENUM_VALUE(e1976Standard,   "1976Standard"),
    AST_ENUM_VALUE(eHarrisPriester, "HarrisPriester"),
    AST_ENUM_VALUE(eJacchia60,      "Jacchia60"),
    AST_ENUM_VALUE(eJacchia70,      "Jacchia70"),
    AST_ENUM_VALUE(eJacchia71,      "Jacchia71"),
    AST_ENUM_VALUE(eJacchiaRoberts, "JacchiaRoberts"),
    AST_ENUM_VALUE(eMSIS1986,       "MSIS1986"),
    AST_ENUM_VALUE(eMSISE1990,      "MSISE1990"),
    AST_ENUM_VALUE(eNRLMSISE2000,   "NRLMSISE2000"),
    AST_ENUM_VALUE(eCIRA72,         "CIRA72"),
    AST_ENUM_VALUE(eDTM2012,        "DTM2012"),
);

std::string toString(EAtmDensityModel model)
{
    return aEnumDescriptor<EAtmDensityModel>().nameOf(model);
}

AST_NAMESPACE_END
