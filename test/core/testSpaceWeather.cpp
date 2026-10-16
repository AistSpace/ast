///
/// @file      testSpaceWeather.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-17
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


#include "ast/SpaceWeather.hpp"
#include "ast/ConstantSpaceWeather.hpp"
#include "ast/Date.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Test.hpp"

#include <cstdio>
#include <fstream>
#include <string>


AST_USING_NAMESPACE

namespace
{

/// @brief 写入一个定宽右对齐的整数字段
void putIntField(std::string& line, int offset, int width, int value)
{
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%*d", width, value);
    line.replace(offset, width, buf);
}

/// @brief 写入一个定宽右对齐的浮点字段
void putDoubleField(std::string& line, int offset, int width, double value)
{
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%*.1f", width, value);
    line.replace(offset, width, buf);
}

/// @brief 按 Celestrak SpaceWx 的定宽列格式拼一行观测数据
/// @details 各字段的列偏移与 SpaceWeather::load() 里 substr 的取值一一对应，
///          改动其中一处必须同步另一处。Kp 列存的是 Kp*10 的整数，与源文件一致。
std::string makeSpaceWxLine(int year, int month, int day,
                            const int kp10[8], const int ap[8],
                            double f10p7Adj)
{
    std::string line(130, ' ');

    putIntField(line, 0, 4, year);
    putIntField(line, 5, 2, month);
    putIntField(line, 8, 2, day);
    putIntField(line, 11, 4, 2556);         // BSRN，本用例不断言
    putIntField(line, 16, 2, 10);           // ND

    int kpSum = 0;
    for (int j = 0; j < 8; ++j)
    {
        putIntField(line, 19 + j * 3, 2, kp10[j]);
        kpSum += kp10[j];
    }
    putIntField(line, 43, 3, kpSum);

    int apSum = 0;
    for (int j = 0; j < 8; ++j)
    {
        putIntField(line, 47 + j * 4, 3, ap[j]);
        apSum += ap[j];
    }

    putIntField(line, 79, 3, apSum / 8);    // ApAvg
    putDoubleField(line, 83, 3, 0.0);       // Cp
    putIntField(line, 87, 1, 0);            // C9
    putIntField(line, 89, 3, 100);          // ISN
    putDoubleField(line, 93, 5, f10p7Adj);
    putIntField(line, 99, 1, 0);            // fluxQualifier
    putDoubleField(line, 101, 5, f10p7Adj);
    putDoubleField(line, 107, 5, f10p7Adj);
    putDoubleField(line, 113, 5, f10p7Adj);
    putDoubleField(line, 119, 5, f10p7Adj);
    putDoubleField(line, 125, 5, f10p7Adj);

    return line;
}

/// @brief 写一份只含两天观测数据的最小 SpaceWx 文件
/// @return 文件路径；写失败返回空串
std::string writeSpaceWxFile(const std::string& filepath)
{
    const int kp10Day1[8] = {10, 20, 30, 40, 50, 60, 70, 80};   // Kp 1.0 ~ 8.0
    const int kp10Day2[8] = {11, 22, 33, 44, 55, 66, 77, 88};   // Kp 1.1 ~ 8.8
    const int ap[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    std::ofstream file(filepath);
    if (!file.is_open())
        return std::string();

    file << "DATATYPE CssiSpaceWeather\n";
    file << "VERSION 1.2\n";
    file << "NUM_OBSERVED_POINTS 2\n";
    file << "BEGIN OBSERVED\n";
    file << makeSpaceWxLine(2026, 10, 13, kp10Day1, ap, 100.0) << "\n";
    file << makeSpaceWxLine(2026, 10, 14, kp10Day2, ap, 102.0) << "\n";
    file << "END OBSERVED\n";
    file.close();
    return filepath;
}

} // namespace

TEST(SpaceWeather, Load)
{
    if(!aIsGithubCI()) GTEST_SKIP();

    SpaceWeather sw;
    errc_t rc = sw.loadDefault();
    EXPECT_TRUE(rc == eNoError);
    {
        int mjd = aDateToMJD(2026, 2, 12);
        auto entry = sw.getEntry(mjd);
        ASSERT_TRUE(entry != nullptr);
        EXPECT_TRUE(entry->mjd == mjd);
        EXPECT_EQ(entry->F10p7Adj, 125.7);
        EXPECT_EQ(entry->F10p7Obs, 129.0);
    }
    {
        int mjd = aDateToMJD(2026, 2, 18);
        auto entry = sw.getEntry(mjd);
        ASSERT_TRUE(entry != nullptr);
        EXPECT_TRUE(entry->mjd == mjd);
        EXPECT_EQ(entry->F10p7Adj, 180.0);
        EXPECT_EQ(entry->F10p7Obs, 184.3);
    }
    {
        int mjd = aDateToMJD(2041, 9, 1);
        auto entry = sw.getEntry(mjd);
        ASSERT_TRUE(entry != nullptr);
        EXPECT_TRUE(entry->mjd == mjd);
        EXPECT_EQ(entry->F10p7Adj, 70.2);
        EXPECT_EQ(entry->F10p7Obs, 68.9);
    }
    {
        int mjd = aDateToMJD(2041, 10, 1);
        auto entry = sw.getEntry(mjd);
        ASSERT_TRUE(entry != nullptr);
        EXPECT_TRUE(entry->mjd == mjd);
        EXPECT_EQ(entry->F10p7Adj, 70.0);
        EXPECT_EQ(entry->F10p7Obs, 69.8);
    }
}


/// @brief 校验 3 小时 Kp 列表：当前块、跨日回溯、超出数据范围
TEST(SpaceWeather, Kp3HourlyList)
{
    const std::string filepath = "temp_space_weather_kp.txt";
    ASSERT_FALSE(writeSpaceWxFile(filepath).empty());

    SpaceWeather sw;
    ASSERT_EQ(sw.load(filepath), eNoError);
    EXPECT_EQ(sw.size(), static_cast<size_t>(2));

    double kpList[30] = {};

    // 无数据、非法回溯块数都返回 0
    SpaceWeather empty;
    EXPECT_EQ(empty.getKp3HourlyList("2026-10-14 00:30:00"_utc, kpList, 8), 0);
    EXPECT_EQ(sw.getKp3HourlyList("2026-10-14 00:30:00"_utc, kpList, 0), 0);

    // 00:30 UTC 落在第 0 个 3 小时块，向前回溯要跨越日界
    const auto tp = "2026-10-14 00:30:00"_utc;
    EXPECT_EQ(sw.getKp3HourlyList(tp, kpList, 3), 3);
    EXPECT_DOUBLE_EQ(kpList[0], 1.1);   // 10-14 00:00~03:00
    EXPECT_DOUBLE_EQ(kpList[1], 8.0);   // 10-13 21:00~24:00
    EXPECT_DOUBLE_EQ(kpList[2], 7.0);   // 10-13 18:00~21:00

    // 23:00 UTC 落在第 7 个块
    EXPECT_EQ(sw.getKp3HourlyList("2026-10-13 23:00:00"_utc, kpList, 4), 4);
    EXPECT_DOUBLE_EQ(kpList[0], 8.0);
    EXPECT_DOUBLE_EQ(kpList[1], 7.0);
    EXPECT_DOUBLE_EQ(kpList[2], 6.0);
    EXPECT_DOUBLE_EQ(kpList[3], 5.0);

    // 回溯越过文件首日：遇到没有数据的日期即停止，共填 10-14 第 0 块 + 10-13 的 8 块
    EXPECT_EQ(sw.getKp3HourlyList(tp, kpList, 30), 9);
    EXPECT_DOUBLE_EQ(kpList[8], 1.0);   // 10-13 00:00~03:00

    std::remove(filepath.c_str());
}


/// @brief 未提供 3 小时 Kp 数据的提供者应回落到基类默认实现（返回 0，表示调用方改用日 Kp）
TEST(SpaceWeather, Kp3HourlyListNotSupported)
{
    ConstantSpaceWeather provider;
    double kpList[8] = {};
    EXPECT_EQ(provider.getKp3HourlyList("2026-10-14 00:30:00"_utc, kpList, 8), 0);
}


GTEST_MAIN();


