#include "AstSPICE/SpiceZpr.hpp"
#include <cmath>
#include <cstdio>

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
    
    // 反向转换：直角坐标转球坐标
    double r_out, lon_out, lat_out;
    recrad(rectan, r_out, lon_out, lat_out);
    
    printf("反向转换: r=%.0f, lon=%.6f, lat=%.6f\n", r_out, lon_out, lat_out);
    
    // 示例：距离-赤经-赤纬转直角坐标
    double range = 1.0;        // 距离（任意单位）
    double ra = AST_PI / 6;   // 赤经 30度
    double dec = AST_PI / 12;  // 赤纬 15度
    
    Vector3d rectan2;
    radrec(range, ra, dec, rectan2);
    
    printf("\n赤经赤纬转换:\n");
    printf("距离=%.1f, RA=%.2f rad, Dec=%.2f rad\n", range, ra, dec);
    printf("直角坐标: X=%.4f, Y=%.4f, Z=%.4f\n", rectan2[0], rectan2[1], rectan2[2]);
    
    return 0;
}
