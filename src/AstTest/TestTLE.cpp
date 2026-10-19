///
/// @file      TestTLE.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-17
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

#include "TestTLE.hpp"
#include "AstCore/TLE.hpp"
#include <string>

AST_NAMESPACE_BEGIN

namespace
{
    
/// @brief 两角之差的最小绝对值（考虑 2π 周期）
double angleDifference(double a, double b)
{
    double diff = std::fmod(a - b, kTwoPI);
    if (diff > kPI)
        diff -= kTwoPI;
    else if (diff < -kPI)
        diff += kTwoPI;
    return std::fabs(diff);
}

}


errc_t aTLERoundTripCheck(const TLE& tle)
{
    std::string line1;
    std::string line2;
    if (errc_t error = aTLEToString(tle, line1, line2))
        return error;

    TLE parsed = TLE::FromLines(line1, line2);

    char classification = tle.classification();
    if (classification == '\0' || classification == ' ')
        classification = 'U';

    // 整数与字符字段要求完全一致
    if (parsed.noradId() != tle.noradId())
        return eErrorInvalidValue;
    if (parsed.classification() != classification)
        return eErrorInvalidValue;
    if (parsed.intlDesignation() != tle.intlDesignation())
        return eErrorInvalidValue;
    if (parsed.elementNumber() != tle.elementNumber())
        return eErrorInvalidValue;
    if (parsed.revNumber() != tle.revNumber())
        return eErrorInvalidValue;
    if (parsed.ephType() != tle.ephType())
        return eErrorInvalidValue;

    // 历元量化到 1e-8 日（约 0.86 ms）
    if (std::fabs(parsed.epochTime().durationFrom(tle.epochTime())) > 1e-3)
        return eErrorInvalidValue;

    // 角度量化到 1e-4 度
    const double angleTolerance = 2e-6;
    if (angleDifference(parsed.inclination(), tle.inclination()) > angleTolerance)
        return eErrorInvalidValue;
    if (angleDifference(parsed.rightAscenOfNode(), tle.rightAscenOfNode()) > angleTolerance)
        return eErrorInvalidValue;
    if (angleDifference(parsed.argOfPerigee(), tle.argOfPerigee()) > angleTolerance)
        return eErrorInvalidValue;
    if (angleDifference(parsed.meanAnomaly(), tle.meanAnomaly()) > angleTolerance)
        return eErrorInvalidValue;

    // 偏心率量化到 1e-7
    if (std::fabs(parsed.eccentricity() - tle.eccentricity()) > 2e-7)
        return eErrorInvalidValue;

    // 平运动量化到 1e-8 圈/天（约 7e-13 rad/s）
    if (std::fabs(parsed.meanMotion() - tle.meanMotion()) > 1e-12)
        return eErrorInvalidValue;

    // nDot/2 量化到 1e-8（原始字段单位）
    if (std::fabs(parsed.meanMotionDot() - tle.meanMotionDot()) > 2e-8)
        return eErrorInvalidValue;

    // 隐含小数点字段保留 5 位有效数字
    double nddotTolerance = std::max(1e-12, std::fabs(tle.motionDotDot()) * 2e-5);
    if (std::fabs(parsed.motionDotDot() - tle.motionDotDot()) > nddotTolerance)
        return eErrorInvalidValue;

    double bstarTolerance = std::max(1e-12, std::fabs(tle.bstar()) * 2e-5);
    if (std::fabs(parsed.bstar() - tle.bstar()) > bstarTolerance)
        return eErrorInvalidValue;

    return eNoError;
}


AST_NAMESPACE_END
