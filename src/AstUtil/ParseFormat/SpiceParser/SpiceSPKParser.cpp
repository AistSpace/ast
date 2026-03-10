///
/// @file      SpiceSPKParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-10
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

#include "SpiceSPKParser.hpp"

AST_NAMESPACE_BEGIN


#pragma pack(push, 1)


/// @brief SPK 段描述符
/// 解包后的 SPK 段摘要信息
struct SPK_Descriptor
{
    double start_time;          // 开始时间 (ET)
    double end_time;            // 结束时间
    int32_t target;             // 目标体 ID
    int32_t center;             // 中心体 ID
    int32_t frame;              // 参考系 ID
    int32_t type;               // 数据类型
    int32_t begin_addr;         // 数据起始字地址
    int32_t end_addr;           // 数据结束字地址
};

static_assert(sizeof(SPK_Descriptor) == sizeof(double) * 2 + sizeof(int32_t) * 6, "SPK_Descriptor size must be 40");


// -----------------------
// 常用 SPK 数据类型内部布局 
// -----------------------


// 类型 2/3 记录头部 (Chebyshev 多项式)
struct SPK_Type2_Record {
    double mid;                 // 区间中点
    double radius;              // 区间半长
    // 之后是系数，数量由记录大小决定
};

// 类型 2 段尾部目录
struct SPK_Type2_Trailer {
    double init;                // 第一个记录的起始时间
    double intlen;              // 记录区间长度
    double rsize;               // 记录大小（双精度个数）
    double n;                   // 记录个数
};

// 类型 5 离散状态 (二体传播)
struct SPK_Type5_State {
    double x, y, z, vx, vy, vz;
};

// 类型 5 段尾部（不含目录）
struct SPK_Type5_Trailer {
    double gm;                  // 引力常数
    double n;                   // 状态个数
};

// 类型 8/9/12/13 状态向量
struct SPK_State {
    double x, y, z, vx, vy, vz;
};

// 类型 8 段尾部
struct SPK_Type8_Trailer {
    double epoch0;              // 第一个状态的时间
    double step;                // 时间步长
    double degree;              // 插值多项式次数
    double n;                   // 状态个数
};

// 类型 10 TLE 数据包 (14 个双精度)
struct SPK_Type10_Packet {
    double ndt20;               // 1st derivative of mean motion/2
    double ndd60;               // 2nd derivative of mean motion/6
    double bstar;               // B* drag term
    double incl;                // inclination
    double node0;               // right ascension of ascending node
    double ecc;                 // eccentricity
    double omega;               // argument of perigee
    double mo;                  // mean anomaly
    double no;                  // mean motion
    double epoch;               // epoch
    double nu_obliquity;        // nutation in obliquity
    double nu_longitude;        // nutation in longitude
    double d_obliquity_dt;      // rate of nutation in obliquity
    double d_longitude_dt;      // rate of nutation in longitude
};

// 类型 10 常量区 (8 个双精度)
struct SPK_Type10_Constants {
    double j2;
    double j3;
    double j4;
    double sqrt_gm;             // sqrt(GM) in ER^3/min^2
    double high_atm_bound;      // high altitude bound (km)
    double low_atm_bound;       // low altitude bound (km)
    double eq_radius;           // Earth equatorial radius (km)
    double dist_units_per_er;   // distance units per Earth radius
};

// 类型 15 进动圆锥曲线 (16 个双精度)
struct SPK_Type15_Data {
    double epoch_peri;          // epoch of periapsis
    double pole[3];             // trajectory pole vector
    double peri_vec[3];          // periapsis unit vector
    double semi_latus;          // semi-latus rectum
    double ecc;                 // eccentricity
    double j2_flag;             // J2 flag (0 or 1)
    double cbody_pole[3];       // central body pole vector
    double gm;                  // central body GM
    double j2;                  // central body J2
    double radius;              // central body equatorial radius
};

// 类型 17 Equinoctial Elements (12 个双精度)
struct SPK_Type17_Data {
    double epoch_peri;
    double semi_major;
    double h, k;                // equinoctial H, K
    double mean_long;           // mean longitude at epoch
    double p, q;                // equinoctial P, Q
    double peri_rate;           // rate of longitude of periapse
    double mean_long_rate;      // mean longitude rate
    double node_rate;           // longitude of ascending node rate
    double pole_ra, pole_dec;   // pole right ascension, declination
};

// 类型 18/19 数据包（子类型0：12个数；子类型1：6个数）
struct SPK_Type18_Packet_Sub0 {
    double pos[3];
    double vel1[3];
    double vel2[3];
    double acc[3];
};

struct SPK_Type18_Packet_Sub1 {
    double pos[3];
    double vel[3];
};

// 类型 18/19 段尾部
struct SPK_Type18_Trailer {
    double subtype;             // 子类型
    double window;              // 窗大小
    double n;                   // 数据包个数
};

// 类型 20 Chebyshev velocity only 记录
struct SPK_Type20_Record {
    // double x_coeff[?];          // 系数个数由记录大小决定
    // double y_coeff[?];
    // double z_coeff[?];
    double x_midpos;            // 中点位置 X
    double y_midpos;
    double z_midpos;
};

// 类型 20 段尾部
struct SPK_Type20_Trailer {
    double dscale;              // 距离缩放因子
    double tscale;              // 时间缩放因子
    double initjd;              // 初始 JD 整数部分
    double initfr;              // 初始 JD 小数部分
    double intlen;              // 记录区间长度（日）
    double rsize;               // 记录大小
    double n;                   // 记录个数
};

#pragma pack(pop)



AST_NAMESPACE_END
