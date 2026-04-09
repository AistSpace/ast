#include "AstSPICE/SpiceZpr.hpp"
#include <cstdio>
#include <cmath>

AST_USING_NAMESPACE

int main()
{
    // 示例：球坐标转直角坐标
    // 地球平均半径约6371km
    double radius = 6371000.0;  // 米
    double lon = AST_PI / 4;    // 45度经度
    double lat = 0.0;           // 赤道
    
    Vector3d rectan;
    latrec(radius, lon, lat, rectan);
    
    printf("球坐标 (r=%.0f m, lon=%.2f rad, lat=%.2f rad)\n", radius, lon, lat);
    printf("直角坐标: X=%.2f, Y=%.2f, Z=%.2f\n", rectan[0], rectan[1], rectan[2]);
    
    // 手动反向转换验证（reclat和recrad未实现，需手动计算）
    double r_verify = rectan.norm();
    double lon_verify = std::atan2(rectan[1], rectan[0]);
    double lat_verify = std::asin(rectan[2] / r_verify);
    
    printf("反向转换: r=%.0f, lon=%.6f, lat=%.6f\n", r_verify, lon_verify, lat_verify);
    
    // 比较原始输入与反向转换结果
    bool r_match = std::abs(r_verify - radius) < 1e-6;
    bool lon_match = std::abs(lon_verify - lon) < 1e-10;
    bool lat_match = std::abs(lat_verify - lat) < 1e-10;
    
    printf("反向验证比较:\n");
    printf("  半径: 原始=%.0f, 反向=%.0f, 一致=%s\n", radius, r_verify, r_match ? "是" : "否");
    printf("  经度: 原始=%.6f, 反向=%.6f, 一致=%s\n", lon, lon_verify, lon_match ? "是" : "否");
    printf("  纬度: 原始=%.6f, 反向=%.6f, 一致=%s\n", lat, lat_verify, lat_match ? "是" : "否");
    printf("整体验证: %s\n", (r_match && lon_match && lat_match) ? "通过" : "失败");
    
    // 示例：距离-赤经-赤纬转直角坐标
    double range = 1.0;        // 距离（任意单位）
    double ra = AST_PI / 6;   // 赤经 30度
    double dec = AST_PI / 12;  // 赤纬 15度
    
    Vector3d rectan2;
    radrec(range, ra, dec, rectan2);
    
    printf("\n赤经赤纬转换:\n");
    printf("输入: 距离=%.1f, RA=%.2f rad, Dec=%.2f rad\n", range, ra, dec);
    printf("直角坐标: X=%.4f, Y=%.4f, Z=%.4f\n", rectan2[0], rectan2[1], rectan2[2]);
    
    // 手动反向转换验证
    double range_verify = rectan2.norm();
    double ra_verify = std::atan2(rectan2[1], rectan2[0]);
    double dec_verify = std::asin(rectan2[2] / range_verify);
    
    printf("反向转换: 距离=%.6f, RA=%.6f, Dec=%.6f\n", range_verify, ra_verify, dec_verify);
    
    // 比较原始输入与反向转换结果
    bool range_match = std::abs(range_verify - range) < 1e-10;
    bool ra_match = std::abs(ra_verify - ra) < 1e-10;
    bool dec_match = std::abs(dec_verify - dec) < 1e-10;
    
    printf("反向验证比较:\n");
    printf("  距离: 原始=%.1f, 反向=%.6f, 一致=%s\n", range, range_verify, range_match ? "是" : "否");
    printf("  赤经: 原始=%.6f, 反向=%.6f, 一致=%s\n", ra, ra_verify, ra_match ? "是" : "否");
    printf("  赤纬: 原始=%.6f, 反向=%.6f, 一致=%s\n", dec, dec_verify, dec_match ? "是" : "否");
    
    bool all_pass = r_match && lon_match && lat_match && range_match && ra_match && dec_match;
    printf("\n整体验证: %s\n", all_pass ? "通过" : "失败");
    
    return all_pass ? 0 : 1;
}
