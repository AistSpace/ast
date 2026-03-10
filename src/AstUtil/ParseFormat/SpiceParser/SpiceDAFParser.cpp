///
/// @file      SpiceDAFParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-09
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

#include "SpiceDAFParser.hpp"
#include <cstdint>
#include <iostream>

AST_NAMESPACE_BEGIN


// 强制 1 字节对齐
#pragma pack(push, 1)

/*!
参考SPICE文档 doc\html\req\daf.html
*/


/// @brief 文件记录 (Record 1, 1024 字节)
struct DAF_FileRecord {
    char    locidw[8];          ///< 标识字，例如 "DAF/SPK"
    int32_t nd;                 ///< 双精度分量个数 ND
    int32_t ni;                 ///< 整型分量个数 NI
    char    locifn[60];         ///< 内部文件名或文件描述
    int32_t fward;              ///< 第一个摘要记录号
    int32_t bward;              ///< 最后一个摘要记录号
    int32_t free;               ///< 第一个空闲字地址
    char    locfmt[8];          ///< 二进制格式 "LTL-IEEE" 或 "BIG-IEEE"
    char    prenul[603];        ///< 填充空字符 (使 ftpstr 位于偏移 699)
    char    ftpstr[28];         ///< FTP 验证字符串
    char    pstnul[297];        ///< 填充至记录末尾 (确保总大小 1024)
};


/// @brief 注释记录
struct DAF_CommentArea{
    char comment[1024];         ///< 注释字符串，最大 1024 字节
};

/// @brief 摘要记录 (每个摘要记录的前 24 字节为头部信息)
struct DAF_SummaryRecords {
    double next;                 ///< 下一个摘要记录号 (存储为 double)
    double prev;                 ///< 上一个摘要记录号 (存储为 double)
    double nsum;                 ///< 本记录中摘要个数 (存储为 double)
    union{
        double  dval[1];         ///< 双精度数据
        int32_t ival[2];         ///< 整数数据
    } summaries[125];            ///< 摘要数据，最多 125 个双精度数或 250 个整型数
};




/// @brief 名称记录
/// 它是一个连续的字符数组，长度 1024 字节。
/// 每个名称长度为 NC = 8 * (ND + (NI+1)/2)，顺序存放
struct DAF_NameRecords{
    char names[1024];           ///< 顺序存放的名称字符串
};


/// @brief 元素记录
/// 它是一个连续的双精度数组，长度 128 字节。
/// 每个元素顺序存放。
struct DAF_ElementRecords{
    double elements[128];       ///< 顺序存放的元素数据
};


// 检查 DAF 文件记录的偏移量和大小是否符合预期

static_assert(offsetof(DAF_FileRecord, locifn) == 16, "offset of locifn must be 16");
static_assert(sizeof(DAF_FileRecord) == 1024, "DAF_FileRecord size must be 1024");
static_assert(sizeof(DAF_SummaryRecords) == 1024, "DAF_SummaryRecords size must be 1024");
static_assert(sizeof(DAF_NameRecords) == 1024, "DAF_NameRecords size must be 1024");
static_assert(sizeof(DAF_ElementRecords) == 1024, "DAF_ElementRecords size must be 1024");



// 类型 21 扩展 MDA 记录
// 记录大小可变，最小 71，最大由 MAXTRM 决定
// 具体布局参照类型 1，但此处不固定

#pragma pack(pop)

err_t SpiceDAFParser::runTest()
{
    if(!file_)
        return -1;
    DAF_FileRecord fileRec;
    DAF_CommentArea commentRec1;
    DAF_CommentArea commentRec2;
    DAF_SummaryRecords summaryRec1;
    DAF_NameRecords nameRec1;
    size_t size;
    size = fread(&fileRec, sizeof(DAF_FileRecord), 1, file_);
    size = fread(&commentRec1, sizeof(DAF_CommentArea), 1, file_);
    size = fread(&commentRec2, sizeof(DAF_CommentArea), 1, file_);
    size = fread(&summaryRec1, sizeof(DAF_SummaryRecords), 1, file_);
    size = fread(&nameRec1, sizeof(DAF_NameRecords), 1, file_);
    //SPK_Descriptor spkDesc[14];
    //memcpy(spkDesc, summaryRec1.summaries, sizeof(SPK_Descriptor) * 14);


    // 假设已根据 fileRec.locfmt 判断字节序，并将整数转换为主机序（此处省略转换代码）
    int nd = fileRec.nd;   // 需转换
    int ni = fileRec.ni;   // 需转换

    int ss = nd + (ni + 1) / 2;     /// 摘要大小 (size of a single summary)
    int ns = 125 / ss;              /// 每个摘要记录可存储的摘要个数 (number of summaries)
    int nc = 8 * ss;                /// 每个名称长度 (number of characters in a single name)

    std::cout << "ND=" << nd << ", NI=" << ni << ", summary size=" << ss << " double, name size=" << nc << std::endl;

    A_UNUSED(size);
    A_UNUSED(ns);
    return 0;
}

AST_NAMESPACE_END


