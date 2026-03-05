///
/// @file      SpiceAxesRegister.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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

#include "SpiceAxesRegistry.hpp"
#include "AstUtil/StringView.hpp"
#include "AstCore/AxesTransform.hpp"


AST_NAMESPACE_BEGIN

SpiceAxesRegistry &SpiceAxesRegistry::Instance()
{
    static SpiceAxesRegistry instance(true);
    return instance;
}

SpiceAxesRegistry::SpiceAxesRegistry(bool whetherInit)
{
    if(whetherInit)
    {
        init();
    }
}


PAxes SpiceAxesRegistry::findAxes(StringView name) const
{
    auto it = axesMap_.find(name.to_string());
    if (it == axesMap_.end())
    {
        return nullptr;
    }
    return it->second;
}

err_t SpiceAxesRegistry::init()
{
    axesMap_["ICRF"] = aAxesICRF();
    axesMap_["ECF"] = aAxesECF();
    axesMap_["J2000"] = aAxesJ2000();
    axesMap_["MOD"] = aAxesMOD();
    axesMap_["TOD"] = aAxesTOD();
    axesMap_["GTOD"] = aAxesGTOD();
    // axesMap_["B1950"] = aAxesB1950();
    axesMap_["B1950"] = aAxesB1950Spice();
    return eNoError;
}

AST_NAMESPACE_END

