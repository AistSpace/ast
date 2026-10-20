///
/// @file      Math.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-19
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

#include "Math.hpp"

AST_NAMESPACE_BEGIN

double round(double x, int ndigits) 
{
    if(ndigits >= 0)
    {
        if(ndigits > 22) 
        {
            double scale1 = std::pow(10.0, ndigits-22);
            double scale2 = 1e22;
            double scaled = (x * scale1) * scale2;
            if (!std::isfinite(scaled))
                return x;
            return (std::round(scaled) / scale2) / scale1;
        }
        else
        {
            double scale = std::pow(10.0, ndigits);
            double scaled = x * scale;
            if (!std::isfinite(scaled))
                return x;
            return std::round(scaled) / scale;
        }
    }
    else
    {
        double scale = std::pow(10.0, -ndigits);
        if (!std::isfinite(scale))
            return 0;
        return std::round(x / scale) * scale;
    }
}


AST_NAMESPACE_END
