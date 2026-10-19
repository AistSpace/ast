///
/// @file      TLE.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-18
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

#include "TLE.hpp"
#include "TLELines.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstUtil/Constants.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Span.hpp"
#include "AstMath/Util.hpp"

#include <cmath>
#include <cstring>

AST_NAMESPACE_BEGIN




namespace {

/// @brief TLE 单行长度
constexpr size_t kTLELineLength = 69;


/// @brief 格式化 NORAD 编号（5 列，支持 Alpha-5）
bool formatTLESatNum(int id, Span<char, 6> out)
{
    if (id < 0 || id > 339999)
        return false;

    if (id < 100000)
    {
        std::snprintf(out.data(), out.size(), "%05d", id);
        return true;
    }

    // Alpha-5：首位字母表示 10-33，跳过 I/O
    int lead = id / 10000;
    int suffix = id % 10000;
    char letter = (char)('A' + (lead - 10));
    if (letter >= 'I')
        ++letter;
    if (letter >= 'O')
        ++letter;
    std::snprintf(out.data(), out.size(), "%c%04d", letter, suffix);
    return true;
}

/// @brief 格式化隐含小数点科学计数法字段 `±NNNNN±N`（8 列）
/// @details 字段值 = ±0.NNNNN × 10^±N，即 TLE 的 B* 与 nDotDot/6 字段。
///          指数只有一位：下溢（指数 < -9）按 0 写出，上溢（指数 > 9）返回失败。
bool formatTLEImpliedExp(double value, Span<char, 9> out)
{
    if (!std::isfinite(value))
        return false;

    double magnitude = std::fabs(value);
    int exponent = 0;
    while (magnitude >= 1.0)
    {
        magnitude /= 10.0;
        ++exponent;
    }
    while (magnitude > 0.0 && magnitude < 0.1)
    {
        magnitude *= 10.0;
        --exponent;
    }

    long long mantissa = (long long)std::llround(magnitude * 1e5);
    if (mantissa >= 100000)
    {
        mantissa = 10000;
        ++exponent;
    }

    // 下溢：一位指数放不下（|value| < 1e-10），按 0 写出
    if (exponent < -9 || value == 0)
    {
        std::snprintf(out.data(), out.size(), " 00000-0");
        return true;
    }

    // 上溢：一位指数放不下，无法表示
    if (exponent > 9)
        return false;

    // 用 signbit 而不是 `value < 0`：-0.0 也要保留负号
    char sign = std::signbit(value) ? '-' : ' ';
    char expSign = (exponent > 0) ? '+' : '-';
    int expAbs = exponent < 0 ? -exponent : exponent;
    std::snprintf(out.data(), out.size(), "%c%05lld%c%d", sign, mantissa, expSign, expAbs);
    return true;
}

/// @brief 格式化 nDot/2 字段（10 列）：符号位 + '.' + 8 位数字
/// @details 不能直接用 `%10.8f`——负值会得到 11 个字符而溢出列宽。
bool formatTLENdot(double value, Span<char, 11> out)
{
    if (!std::isfinite(value) || std::fabs(value) >= 1.0)
        return false;

    long long digits = (long long)std::llround(std::fabs(value) * 1e8);
    if (digits > 99999999)
        digits = 99999999;

    // 用 signbit 而不是 `value < 0`：-0.0 也要保留负号（"- .00000000" 往返一致）
    char sign = std::signbit(value) ? '-' : ' ';
    std::snprintf(out.data(), out.size(), "%c.%08lld", sign, digits);
    return true;
}

/// @brief 格式化角度字段（度，固定 4 位小数，8 列）
bool formatTLEDegrees(double radians, Span<char, 9> out)
{
    radians = aNormalizeAngle0To2Pi(radians);
    double degrees = radians * kRadToDeg;
    // 宽度 8、空格填充（不能用 `%08.4f`，那会补零成 `051.6416`）
    int length = std::snprintf(out.data(), out.size(), "%8.4f", degrees);
    return length == out.size() - 1;
}


/// @brief 格式化 TLE 历元字段 `YYDDD.DDDDDDDD`（14 列 + 结尾 '\0'）
errc_t formatTLEEpochField(const TimePoint& time, Span<char, 15> out)
{
    DateTime dttm;
    aTimePointToUTC(time, dttm);

    int year = dttm.year();
    // 年内日（1 起算）+ 当日小数部分
    double dayFraction = (double)dttm.dayOfYear() + dttm.time().dayFraction();
    // 按输出精度（8 位小数日）取整；年末可能进位到下一年第 1 天
    dayFraction = round(dayFraction, 8);
    double daysInYear = aIsLeapYear(year) ? 366.0 : 365.0;
    if (dayFraction > daysInYear)
    {
        dayFraction -= daysInYear;
        ++year;
    }
    if (year < 1957 || year > 2056)
        return eErrorOutOfRange;

    int length = std::snprintf(out.data(), out.size(), "%02d%012.8f", year % 100, dayFraction);
    if (length != 14)
        return eErrorInvalidValue;

    return eNoError;
}


/// @brief 解析 TLE 科学记数法字段（如二阶导数、BSTAR），返回合值
/// @details 字段格式为 [符号位][5 位小数][指数符号][指数数字]，假设前导小数点。
///          例如 " 10191-3" → 0.10191 × 10⁻³ = 0.00010191。
/// @param raw 8 字符字段
/// @return 合并后的 double 值；字段长度不足时返回 0
double parseTLEExpField(StringView raw)
{
    if (raw.size() < 8)
        return 0.0;

    // 尾数：[0] 符号位, [1-5] 5 位小数，假设前导小数点；TLE 惯例空格视为 '0'
    char mantissa[8] = "0.";
    for (size_t i = 0; i < 5; ++i)
    {
        char c = raw[1 + i];
        mantissa[2 + i] = (c == ' ') ? '0' : c;
    }
    double value = aParseDouble(StringView(mantissa));
    if (raw[0] == '-')
        value = -value;

    // 指数：[6] 符号, [7] 数字（空格→0）
    char expDigit = (raw[7] == ' ') ? '0' : raw[7];
    int exponent = expDigit - '0';
    if (raw[6] == '-')
        exponent = -exponent;

    return value * pow(10.0, exponent);
}

} // namespace

/// @brief 解析 5 位 NORAD 目录编号（支持 Alpha-5 字母扩展）
/// @details 首位为字母（A-Z，跳过 I/O）时按 Alpha-5 解码：编号 = 首位值 × 10000 + 后 4 位；
///          首位为数字或空格时按传统整数解析。
int aParseNoradId(StringView str)
{
    if (str.empty())
        return 0;

    char c = str[0];
    // Alpha-5：首位为字母（A-Z，跳过 I/O）
    if (c >= 'A' && c <= 'Z')
    {
        int lead = c - 'A' + 10;
        if (c > 'I') --lead;   // 跳过 I
        if (c > 'O') --lead;   // 跳过 O
        int suffix = 0;
        if (aParseInt(str.substr(1), suffix) != eNoError)
            return 0;
        return lead * 10000 + suffix;
    }

    // 传统：数字/右对齐空格（aParseInt 内部跳过前导空白）
    int value = 0;
    if (aParseInt(str, value) != eNoError)
        return 0;
    return value;
}


TLE TLE::FromLines(const TLELines &lines)
{
    StringView l1 = lines.line1();
    StringView l2 = lines.line2();
    return FromLines(l1, l2);
}

TLE TLE::FromLines(StringView l1, StringView l2)
{
    TLE tle{};
    if(l1.size() < 69 || l2.size() < 69)
    {
        aInfo(_("TLE 行长度不足 69 字符"));
        return tle;
    }

    // ---- 解析第1行（69 字符）----
    {
        // 列 03-07: NORAD 目录编号 (1-based 3-7 → 0-based 2-6)
        // 支持 Alpha-5 字母扩展与数字/空格右对齐
        tle.noradId_ = aParseNoradId(l1.substr(2, 5));

        // 列 08: 密级分类 (1-based 8 → 0-based 7)
        tle.classification_ = l1[7];

        // 列 10-17: 国际编号 (1-based 10-17 → 0-based 9-16)，去掉尾部空格
        tle.intlDesignation_ = std::string(aStripTrailingAsciiWhitespace(l1.substr(9, 8)));

        // 列 19-20: 历元年份（年份后两位）
        int epochYear = aParseInt(l1.substr(18, 2));

        // 列 21-32: 历元（一年中的第几天及小数部分）
        double epochDays = aParseDouble(l1.substr(20, 12));

        tle.epochTime_ = TimePoint::FromTLEYD(epochYear, epochDays);

        // 列 34-43: 平均运动的一阶时间导数 [圈/天²]
        tle.meanMotionDotTime_ = aParseDouble(l1.substr(33, 10));

        // 列 45-52: 平均运动的二阶时间导数（科学记数法，假设前导小数点）
        tle.motionDotDot_ = parseTLEExpField(l1.substr(44, 8));

        // 列 54-61: BSTAR 阻力项（格式同上）
        tle.bstar_ = parseTLEExpField(l1.substr(53, 8));

        // 列 63: 星历表类型
        tle.ephType_ = aParseInt(l1.substr(62, 1));

        // 列 65-68: 轨道根数编号
        tle.elementNumber_ = aParseInt(l1.substr(64, 4));
    }

    // ---- 解析第2行（69 字符）----
    {
        // 列 09-16: 轨道倾角 [度] → 弧度
        tle.inclination_ = aParseDouble(l2.substr(8, 8)) * kDegToRad;

        // 列 18-25: 升交点赤经 [度] → 弧度
        tle.rightAscenOfNode_ = aParseDouble(l2.substr(17, 8)) * kDegToRad;

        // 列 27-33: 偏心率（假设前导小数点，7 位数字）
        tle.eccentricity_ = aParseInt(l2.substr(26, 7)) * 1e-7;

        // 列 35-42: 近地点幅角 [度] → 弧度
        tle.argOfPerigee_ = aParseDouble(l2.substr(34, 8)) * kDegToRad;

        // 列 44-51: 平近点角 [度] → 弧度
        tle.meanAnomaly_ = aParseDouble(l2.substr(43, 8)) * kDegToRad;

        // 列 53-63: 平均运动 [圈/天] → rad/s
        tle.meanMotion_ = aParseDouble(l2.substr(52, 11)) * kTwoPI / 86400.0;

        // 列 64-68: 历元时刻的已转圈数
        tle.revNumber_ = aParseInt(l2.substr(63, 5));
    }
    return tle;
}

std::string TLE::alpha5() const
{
    char buffer[6] = {};
    if(errc_t rc = formatTLESatNum(noradId(), buffer))
    {
        aWarning(_("编号 %d 无法转换为 Alpha-5 编号"), noradId());
        return std::string();
    }
    return std::string(buffer);
}

void TLE::setAlpha5(StringView v)
{
    noradId_ = aParseNoradId(v);
}

TLELines TLE::toLines() const
{
    TLELines lines;
    lines.name() = name();
    aTLEToString(*this, lines.line1(), lines.line2());
    return lines;
}


errc_t aFormatTLEEpoch(const TimePoint& time, std::string& out)
{
    char buffer[15] = {};
    if (errc_t error = formatTLEEpochField(time, buffer))
        return error;

    out.assign(buffer);
    return eNoError;
}

int aTLEChecksum(StringView line)
{
    int sum = 0;
    size_t count = line.size() < kTLELineLength - 1 ? line.size() : kTLELineLength - 1;
    for (size_t i = 0; i < count; ++i)
    {
        char c = line[i];
        if (c >= '0' && c <= '9')
            sum += c - '0';
        else if (c == '-')
            sum += 1;
    }
    return sum % 10;
}

/// @brief 将 TLE 格式化为两行文本（写入固定长度缓冲区）
/// @details 每个输出 69 字符 + 结尾 '\0'；各字段通过编译期 `subspan` 直接写进目标列，
///          字段格式化函数要求的 extent 即"列宽 + 1"
///         （末尾的 `\0` 随后会被下一个字段或分隔空格覆盖）
///          出错时输出内容可能只写了一部分
errc_t aTLEToString(const TLE& tle, Span<char, 70> line1, Span<char, 70> line2)
{
    std::memset(line1.data(), 0, line1.size());
    std::memset(line2.data(), 0, line2.size());

    // ---- 第 1 行 ----
    line1[0] = '1';
    line1[1] = ' ';

    // 列 03-07: NORAD 编号（Alpha-5 字母占第 3 列）
    if (!formatTLESatNum(tle.noradId(), line1.subspan<2, 6>()))
        return eErrorOutOfRange;

    // 列 08: 密级
    char classification = tle.classification();
    if (classification == '\0' || classification == ' ')
        classification = 'U';
    line1[7] = classification;
    line1[8] = ' ';

    // 列 10-17: 国际编号（左对齐补空格）
    const std::string& intlDesignation = tle.intlDesignation();
    if (intlDesignation.size() > 8)
        return eErrorOutOfRange;
    Span<char, 8> intlField = line1.subspan<9, 8>();
    std::memset(intlField.data(), ' ', intlField.size());
    std::memcpy(intlField.data(), intlDesignation.data(), intlDesignation.size());
    line1[17] = ' ';

    // 列 19-32: 历元
    if (errc_t error = formatTLEEpochField(tle.epochTime(), line1.subspan<18, 15>()))
        return error;
    line1[32] = ' ';

    // 列 34-43: 平均运动一阶导数
    if (!formatTLENdot(tle.meanMotionDot(), line1.subspan<33, 11>()))
        return eErrorOutOfRange;
    line1[43] = ' ';

    // 列 45-52: 平均运动二阶导数
    if (!formatTLEImpliedExp(tle.motionDotDot(), line1.subspan<44, 9>()))
        return eErrorOutOfRange;
    line1[52] = ' ';

    // 列 54-61: BSTAR
    if (!formatTLEImpliedExp(tle.bstar(), line1.subspan<53, 9>()))
        return eErrorOutOfRange;
    line1[61] = ' ';

    // 列 63: 星历表类型
    int ephType = tle.ephType();
    if (ephType < 0 || ephType > 9)
        return eErrorOutOfRange;
    line1[62] = (char)('0' + ephType);
    line1[63] = ' ';

    // 列 65-68: 轨道根数编号
    int elementNumber = tle.elementNumber();
    if (elementNumber < 0 || elementNumber > 9999)
        return eErrorOutOfRange;
    Span<char, 5> elementField = line1.subspan<64, 5>();
    if (std::snprintf(elementField.data(), elementField.size(), "%4d", elementNumber) != 4)
        return eErrorInvalidValue;

    // 列 69: 校验和
    line1[68] = (char)('0' + aTLEChecksum(StringView(line1.data(), kTLELineLength - 1)));

    // ---- 第 2 行 ----
    line2[0] = '2';
    line2[1] = ' ';

    // 列 03-07: NORAD 编号
    if (!formatTLESatNum(tle.noradId(), line2.subspan<2, 6>()))
        return eErrorOutOfRange;
    line2[7] = ' ';

    // 列 09-16: 轨道倾角
    if (!formatTLEDegrees(tle.inclination(), line2.subspan<8, 9>()))
        return eErrorOutOfRange;
    line2[16] = ' ';

    // 列 18-25: 升交点赤经
    if (!formatTLEDegrees((tle.rightAscenOfNode()), line2.subspan<17, 9>()))
        return eErrorOutOfRange;
    line2[25] = ' ';

    // 列 27-33: 偏心率（7 位数字，隐含前导小数点）
    double eccentricity = tle.eccentricity();
    if (!(eccentricity >= 0.0 && eccentricity < 1.0))
        return eErrorOutOfRange;
    long long eccDigits = (long long)std::llround(eccentricity * 1e7);
    if (eccDigits > 9999999)
        eccDigits = 9999999;
    Span<char, 8> eccentricityField = line2.subspan<26, 8>();
    if (std::snprintf(eccentricityField.data(), eccentricityField.size(), "%07lld", eccDigits) != 7)
        return eErrorInvalidValue;
    line2[33] = ' ';

    // 列 35-42: 近地点幅角
    if (!formatTLEDegrees((tle.argOfPerigee()), line2.subspan<34, 9>()))
        return eErrorOutOfRange;
    line2[42] = ' ';

    // 列 44-51: 平近点角
    if (!formatTLEDegrees((tle.meanAnomaly()), line2.subspan<43, 9>()))
        return eErrorOutOfRange;
    line2[51] = ' ';

    // 列 53-63: 平均运动（圈/天，8 位小数）
    double revPerDay = tle.meanMotion() * kDayToSec / kTwoPI;
    if (!(revPerDay >= 0.0 && revPerDay < 100.0))
        return eErrorOutOfRange;
    Span<char, 12> meanMotionField = line2.subspan<52, 12>();
    if (std::snprintf(meanMotionField.data(), meanMotionField.size(), "%11.8f", revPerDay) != 11)
        return eErrorOutOfRange;

    // 列 64-68: 历元时刻的已转圈数
    int rev = tle.revNumber();
    if (rev < 0 || rev > 99999)
        return eErrorOutOfRange;
    Span<char, 6> revNumberField = line2.subspan<63, 6>();
    if (std::snprintf(revNumberField.data(), revNumberField.size(), "%5d", rev) != 5)
        return eErrorInvalidValue;

    // 列 69: 校验和
    line2[68] = (char)('0' + aTLEChecksum(StringView(line2.data(), kTLELineLength - 1)));

    return eNoError;
}

errc_t aTLEToString(const TLE& tle, std::string& line1, std::string& line2)
{
    char line1Buffer[kTLELineLength + 1] = {};
    char line2Buffer[kTLELineLength + 1] = {};
    if (errc_t error = aTLEToString(tle, line1Buffer, line2Buffer))
        return error;

    line1.assign(line1Buffer, kTLELineLength);
    line2.assign(line2Buffer, kTLELineLength);
    return eNoError;
}



void aTLEToOrbElem(const TLE &tle, double gm, OrbElem &elem)
{
    elem.a_      = aMeanMotionToSMA(tle.meanMotion(), gm);
    elem.e_      = tle.eccentricity();
    elem.i_      = tle.inclination();
    elem.raan_   = tle.rightAscenOfNode();
    elem.argper_ = tle.argOfPerigee();
    elem.trueA_  = aMeanToTrue(tle.meanAnomaly(), tle.eccentricity());
}

OrbElem aTLEToOrbElem(const TLE& tle, double gm)
{
    OrbElem elem;
    aTLEToOrbElem(tle, gm, elem);
    return elem;
}

void aTLEToModOrbElem(const TLE &tle, double gm, ModOrbElem &elem)
{
    elem.rp_     = aMeanMotionToPeriRad(tle.meanMotion(), tle.eccentricity(), gm);
    elem.e_      = tle.eccentricity();
    elem.i_      = tle.inclination();
    elem.raan_   = tle.rightAscenOfNode();
    elem.argper_ = tle.argOfPerigee();
    elem.trueA_  = aMeanToTrue(tle.meanAnomaly(), tle.eccentricity());
}

ModOrbElem aTLEToModOrbElem(const TLE& tle, double gm)
{
    ModOrbElem elem;
    aTLEToModOrbElem(tle, gm, elem);
    return elem;
}

AST_NAMESPACE_END
