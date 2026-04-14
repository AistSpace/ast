///
/// @file      MotionExternalEphemeris.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-14
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

#pragma once

#include "AstGlobal.h"
#include "AstSim/MotionProfile.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

enum class EEphemerisType
{
    eSTKEphemeris,          ///< STK格式星历
    eSTKBinaryEphemeris,    ///< STK二进制格式星历
    eCCSDSEphemeris,        ///< CCSDS格式星历
    eITCEphemeris,          ///< ITC格式星历
};

class Mover;

/// @brief 外部星历运动模型
class AST_SIM_API MotionExternalEphemeris: public MotionProfile
{
public:
    static MotionExternalEphemeris* New();
    MotionExternalEphemeris() = default;
    ~MotionExternalEphemeris() = default;
public:
    errc_t makeEphemerisSpec(ScopedPtr<Ephemeris>& eph) const override;
    errc_t makeEphemerisSimple(ScopedPtr<Ephemeris>& eph) const override;
    void accept(MotionProfileVisitor& visitor) override;
public:
    /// @brief 获取星历类型
    /// @return 星历类型
    EEphemerisType getEphemerisType() const{return ephemerisType_;}

    /// @brief 设置星历类型
    /// @param type 星历类型
    void setEphemerisType(EEphemerisType type){ephemerisType_ = type;}

    /// @brief 获取星历文件路径
    /// @return 星历文件路径
    const std::string& getFilePath() const{return filepath_;}

    /// @brief 设置星历文件路径
    /// @param filepath 星历文件路径
    void setFilePath(StringView filepath){filepath_ = std::string(filepath);}

    /// @brief 为指定移动体重新加载星历
    /// @param mover 移动体
    /// @return 错误码
    errc_t reloadEphemerisFor(Mover& mover);
private:
    EEphemerisType ephemerisType_{EEphemerisType::eSTKEphemeris};    ///< 星历类型
    std::string filepath_{};                                         ///< 星历文件路径
};


/*! @} */

AST_NAMESPACE_END
