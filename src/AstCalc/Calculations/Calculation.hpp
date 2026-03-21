///
/// @file      Calculation.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-02-08
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

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN


/// @brief 计算量基类
/// 该类是一个抽象类，不能直接实例化
/// 具体的计算量类需要继承该类，并实现evaluate方法
/// @note
/// 该模块的类型与参数定义参考了NASA的[WebGeoCalc](https://wgc.jpl.nasa.gov:8443/webgeocalc/)软件的接口设计与相关功能
/// 详细也可以参考WebGeoCalc的[python接口文档](https://webgeocalc.readthedocs.io/en/latest/)
class AST_CALC_API Calculation
{
public:
    Calculation() = default;
    virtual ~Calculation() = default;
    virtual err_t evaluate(const TimePoint& tp, double& result) const = 0;
};


AST_NAMESPACE_END
