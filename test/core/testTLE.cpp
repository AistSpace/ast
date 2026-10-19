///
/// @file      testTLE.cpp
/// @brief     TLE 测试
/// @details   验证：
///            1. TLE 两行文本解析（元数据、非开普勒项、历元、轨道根数）
///            2. Alpha-5 编号解析与隐含小数点科学计数法字段
///            3. 模 10 校验和
///            4. 已知 TLE 的逐字节往返
///            5. 历元 YYDDD.DDDDDDDD 格式化与年末进位
///            6. 负值 nDot 的 10 列字段
///            7. Alpha-5 编号、国际编号的写出
///            8. 往返自检与错误码
/// @author    axel
/// @date      2026-10-17
/// @copyright 版权所有 (C) 2026-present, ast项目.

#include "ast/AstTestMacro.h"
#include "ast/TLE.hpp"
#include "ast/TLELines.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Constants.hpp"
#include "ast/ParseFormat.hpp"
#include "ast/RunTime.hpp"
#include "ast/TestTLE.hpp"
#include "ast/TLEDatabase.hpp"

#include <cmath>
#include <string>

static const char* kLine1 = "1 26661U 99057LT  21084.99115140  .00000893  00000-0  18006-3 0  9994";
static const char* kLine2 = "2 26661  98.3488 112.3622 0070611 169.6757 257.7924 14.62494120100130";

AST_USING_NAMESPACE

/// @brief 从两行字符串创建 TLELines
static TLELines makeTLELines()
{
    TLELines lines;
    lines.line1() = kLine1;
    lines.line2() = kLine2;
    return lines;
}

/// @brief 替换 TLE 行中指定位置、指定宽度的字段
static std::string replaceField(const char* line, size_t pos, size_t length, const char* field)
{
    std::string result(line);
    result.replace(pos, length, field);
    return result;
}


TEST(TLEParserTest, Line1Metadata)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());

    EXPECT_EQ(tle.noradId(), 26661);
    EXPECT_EQ(tle.classification(), 'U');
    // 国际编号列左对齐、尾部补空格，解析时应去掉尾部空格
    EXPECT_EQ(tle.intlDesignation(), "99057LT");
    EXPECT_EQ(tle.ephType(), 0);
    EXPECT_EQ(tle.elementNumber(), 999);
}


TEST(TLEParserTest, NonKeplerianFields)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());

    // 第 34-43 列：nDot/2，正数带前导空格
    EXPECT_NEAR(tle.meanMotionDot(), 8.93e-6, 1e-15);
    // 第 45-52 列：二阶导数，隐含前导小数点
    EXPECT_NEAR(tle.motionDotDot(), 0.0, 1e-15);
    // 第 54-61 列：B* = 0.18006 × 10⁻³
    EXPECT_NEAR(tle.bstar(), 1.8006e-4, 1e-15);
}


TEST(TLEParserTest, ImpliedDecimalFields)
{
    AST_USING_NAMESPACE
    // nddot = 0.12345 × 10⁻⁴，bstar = -0.98765 × 10⁻³
    std::string l1 = replaceField(kLine1, 44, 8, " 12345-4");
    l1 = replaceField(l1.c_str(), 53, 8, "-98765-3");
    // 负值 nDot 字段
    l1 = replaceField(l1.c_str(), 33, 10, "-.00000893");

    TLE tle = TLE::FromLines(l1, kLine2);
    EXPECT_NEAR(tle.meanMotionDot(), -8.93e-6, 1e-15);
    EXPECT_NEAR(tle.motionDotDot(), 1.2345e-5, 1e-15);
    EXPECT_NEAR(tle.bstar(), -9.8765e-4, 1e-15);
}


TEST(TLEParserTest, Epoch)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());

    // 2021 年第 84.99115140 日 = 2021-03-25 23:47:15.48 UTC
    TimePoint expected = TimePoint::FromUTC(2021, 3, 25, 0, 0, 0)
                       + 0.99115140 * kDayToSec;
    EXPECT_LT(std::fabs(tle.epochTime().durationFrom(expected)), 1e-6);
}


TEST(TLEParserTest, EpochYearPivot)
{
    AST_USING_NAMESPACE
    // YY < 57 → 20YY，YY >= 57 → 19YY
    std::string l1 = replaceField(kLine1, 18, 14, "57001.00000000");
    TLE tle = TLE::FromLines(l1, kLine2);
    TimePoint expected = TimePoint::FromUTC(1957, 1, 1, 0, 0, 0);
    EXPECT_LT(std::fabs(tle.epochTime().durationFrom(expected)), 1e-6);

    l1 = replaceField(kLine1, 18, 14, "56001.00000000");
    tle = TLE::FromLines(l1, kLine2);
    expected = TimePoint::FromUTC(2056, 1, 1, 0, 0, 0);
    EXPECT_LT(std::fabs(tle.epochTime().durationFrom(expected)), 1e-6);
}


TEST(TLEParserTest, Line2Elements)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());

    EXPECT_NEAR(tle.inclination(), 98.3488 * kDegToRad, 1e-12);
    EXPECT_NEAR(tle.rightAscenOfNode(), 112.3622 * kDegToRad, 1e-12);
    // 偏心率列省略前导小数点
    EXPECT_NEAR(tle.eccentricity(), 0.0070611, 1e-15);
    EXPECT_NEAR(tle.argOfPerigee(), 169.6757 * kDegToRad, 1e-12);
    EXPECT_NEAR(tle.meanAnomaly(), 257.7924 * kDegToRad, 1e-12);
    // 平均运动由 圈/天 换算为 rad/s
    EXPECT_NEAR(tle.meanMotion(), 14.62494120 * kTwoPI / kDayToSec, 1e-18);
    EXPECT_EQ(tle.revNumber(), 10013);
}


TEST(TLEParserTest, Alpha5NoradId)
{
    AST_USING_NAMESPACE
    EXPECT_EQ(aParseNoradId(StringView("26661")), 26661);
    // 数字/空格右对齐
    EXPECT_EQ(aParseNoradId(StringView(" 1234")), 1234);
    EXPECT_EQ(aParseNoradId(StringView("A0000")), 100000);
    EXPECT_EQ(aParseNoradId(StringView("H9999")), 179999);
    // 跳过 I 和 O
    EXPECT_EQ(aParseNoradId(StringView("J0000")), 180000);
    EXPECT_EQ(aParseNoradId(StringView("P0000")), 230000);
    EXPECT_EQ(aParseNoradId(StringView("Z9999")), 339999);
    EXPECT_EQ(aParseNoradId(StringView("")), 0);

    // 整行解析：C3456 → 123456
    std::string l1 = replaceField(kLine1, 2, 5, "C3456");
    TLE tle = TLE::FromLines(l1, kLine2);
    EXPECT_EQ(tle.noradId(), 123456);
}


TEST(TLEParserTest, TruncatedLines)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines("1 26661U", "2 26661");

    // 不足 69 字符时不解析字段，保持默认值，但原始行文本仍保留
    EXPECT_EQ(tle.noradId(), 0);
    EXPECT_EQ(tle.epochTime(), TimePoint{});
    EXPECT_EQ(tle.eccentricity(), 0.0);
    EXPECT_EQ(tle.meanMotion(), 0.0);
}


TEST(TLEWriterTest, Checksum)
{
    AST_USING_NAMESPACE
    EXPECT_EQ(aTLEChecksum(StringView(kLine1)), 4);
    EXPECT_EQ(aTLEChecksum(StringView(kLine2)), 0);

    // 减号按 1 计、字母/空格/句点/加号按 0 计
    EXPECT_EQ(aTLEChecksum(StringView("-")), 1);
    EXPECT_EQ(aTLEChecksum(StringView("A .+")), 0);
    EXPECT_EQ(aTLEChecksum(StringView("19")), 10 % 10);
}


TEST(TLEWriterTest, ByteExactRoundTrip)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());

    std::string line1;
    std::string line2;
    ASSERT_EQ(aTLEToString(tle, line1, line2), eNoError);

    EXPECT_EQ(line1, kLine1);
    EXPECT_EQ(line2, kLine2);
}


TEST(TLEWriterTest, RoundTripCheck)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());
    EXPECT_EQ(aTLERoundTripCheck(tle), eNoError);
}


TEST(TLEWriterTest, EpochFormat)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());

    std::string epoch;
    ASSERT_EQ(aFormatTLEEpoch(tle.epochTime(), epoch), eNoError);
    EXPECT_EQ(epoch, "21084.99115140");
}


TEST(TLEWriterTest, EpochYearEndRollover)
{
    AST_USING_NAMESPACE
    // 2021 年 12 月 31 日 23:59:59.99999999，量化到 8 位小数日后应进位到 2022 年第 1 天
    TimePoint time = TimePoint::FromUTC(2021, 12, 31, 23, 59, 59.99999999);

    std::string epoch;
    ASSERT_EQ(aFormatTLEEpoch(time, epoch), eNoError);
    EXPECT_EQ(epoch, "22001.00000000");
}


TEST(TLEWriterTest, EpochOutOfRange)
{
    AST_USING_NAMESPACE
    std::string epoch;
    EXPECT_EQ(aFormatTLEEpoch(TimePoint::FromUTC(2057, 1, 1, 0, 0, 0), epoch), eErrorOutOfRange);
    EXPECT_EQ(aFormatTLEEpoch(TimePoint::FromUTC(1956, 12, 31, 0, 0, 0), epoch), eErrorOutOfRange);
}


TEST(TLEWriterTest, ParseNdotWithLeadingSpace)
{
    AST_USING_NAMESPACE
    // nDot 字段的经典写法：符号位 + '.' + 8 位数字，正数带前导空格
    EXPECT_NEAR(aParseDouble(StringView(" .00000893")), 8.93e-6, 1e-15);
    EXPECT_NEAR(aParseDouble(StringView("-.00001001")), -1.001e-5, 1e-15);
}


TEST(TLEWriterTest, NegativeNdotField)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());
    tle.setMeanMotionDot(-8.93e-6);

    std::string line1;
    std::string line2;
    ASSERT_EQ(aTLEToString(tle, line1, line2), eNoError);
    ASSERT_EQ(line1.size(), 69u);

    // 第 34-43 列（0-based 33-42）
    EXPECT_EQ(line1.substr(33, 10), "-.00000893");
    EXPECT_EQ(aTLERoundTripCheck(tle), eNoError);
}


TEST(TLEWriterTest, Alpha5SatelliteNumber)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());
    tle.setNoradId(123456);

    std::string line1;
    std::string line2;
    ASSERT_EQ(aTLEToString(tle, line1, line2), eNoError);

    // 第 3-7 列（0-based 2-6）
    EXPECT_EQ(line1.substr(2, 5), "C3456");

    TLE parsed = TLE::FromLines(line1, line2);
    EXPECT_EQ(parsed.noradId(), 123456);
}


TEST(TLEWriterTest, Alpha5Text)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());

    // 传统编号：5 位数字，左补零
    EXPECT_EQ(tle.alpha5(), "26661");
    tle.setNoradId(1234);
    EXPECT_EQ(tle.alpha5(), "01234");

    // Alpha-5：A=10 起，跳过 I 和 O
    tle.setNoradId(100000);
    EXPECT_EQ(tle.alpha5(), "A0000");
    tle.setNoradId(179999);
    EXPECT_EQ(tle.alpha5(), "H9999");
    tle.setNoradId(180000);
    EXPECT_EQ(tle.alpha5(), "J0000");
    tle.setNoradId(230000);
    EXPECT_EQ(tle.alpha5(), "P0000");
    tle.setNoradId(339999);
    EXPECT_EQ(tle.alpha5(), "Z9999");
}

TEST(TLEWriterTest, Alpha5OutOfRange)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());

    // 超出 0..339999 的编号无法表示为 Alpha-5
    tle.setNoradId(340000);
    EXPECT_TRUE(tle.alpha5().empty());
    tle.setNoradId(-1);
    EXPECT_TRUE(tle.alpha5().empty());
}

TEST(TLEWriterTest, Alpha5RoundTrip)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());

    tle.setAlpha5("C3456");
    EXPECT_EQ(tle.noradId(), 123456);
    EXPECT_EQ(tle.alpha5(), "C3456");
}

TEST(TLEWriterTest, InternationalDesignation)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());
    EXPECT_EQ(tle.intlDesignation(), "99057LT");

    tle.setIntlDesignation("98067A");
    std::string line1;
    std::string line2;
    ASSERT_EQ(aTLEToString(tle, line1, line2), eNoError);

    // 第 10-17 列（0-based 9-16），左对齐补空格
    EXPECT_EQ(line1.substr(9, 8), "98067A  ");

    TLE parsed = TLE::FromLines(line1, line2);
    EXPECT_EQ(parsed.intlDesignation(), "98067A");
    EXPECT_EQ(aTLERoundTripCheck(tle), eNoError);
}


TEST(TLEWriterTest, InvalidFields)
{
    AST_USING_NAMESPACE
    TLE tle = TLE::FromLines(makeTLELines());
    std::string line1;
    std::string line2;

    TLE invalidEccentricity = tle;
    invalidEccentricity.setEccentricity(1.0);
    EXPECT_EQ(aTLEToString(invalidEccentricity, line1, line2), eErrorOutOfRange);

    TLE tinyBstar = tle;
    tinyBstar.setBstar(1e-12);
    // 指数 < -9 时一位指数放不下，按 0 写出
    EXPECT_EQ(aTLEToString(tinyBstar, line1, line2), eNoError);
    EXPECT_EQ(line1.substr(53, 8), " 00000-0");

    TLE hugeBstar = tle;
    hugeBstar.setBstar(1e10);
    // 指数 > 9 无法用 ±0.NNNNN×10^N（指数一位）表示
    EXPECT_EQ(aTLEToString(hugeBstar, line1, line2), eErrorOutOfRange);

    TLE longDesignation = tle;
    longDesignation.setIntlDesignation("123456789");
    EXPECT_EQ(aTLEToString(longDesignation, line1, line2), eErrorOutOfRange);
}


TEST(TLEWriterTest, LoadDatabase)
{
    TLEDatabase database;
    errc_t rc = database.load("./test-data/tle/TLE20230212.txt");
    EXPECT_EQ(rc, eNoError);
    EXPECT_EQ(database.lines().size(), 24902);
    for(auto& tlelines: database.lines())
    {
        EXPECT_EQ(aTLELinesRoundTripCheck(tlelines), eNoError) << tlelines.name();
    }
}

GTEST_MAIN()
