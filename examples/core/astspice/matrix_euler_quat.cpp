#include "AstSPICE/SpiceZpr.hpp"
#include <cstdio>
#include <cmath>

AST_USING_NAMESPACE

int main()
{
    // 示例：欧拉角转旋转矩阵
    double angle1 = AST_PI / 6;  // 30度
    double angle2 = AST_PI / 4;  // 45度
    double angle3 = AST_PI / 3;  // 60度
    
    Matrix3d rotation;
    eul2m(angle3, angle2, angle1, 3, 2, 1, rotation);
    
    printf("欧拉角 (3-2-1顺序):\n");
    printf("输入: angle1=%.6f rad, angle2=%.6f rad, angle3=%.6f rad\n", angle1, angle2, angle3);
    
    // 示例：旋转矩阵转欧拉角
    double ang1_out, ang2_out, ang3_out;
    m2eul(rotation, 3, 2, 1, ang1_out, ang2_out, ang3_out);
    
    printf("输出: angle1=%.6f rad, angle2=%.6f rad, angle3=%.6f rad\n", ang1_out, ang2_out, ang3_out);
    
    // 比较输入与输出
    bool eul_match = std::abs(angle1 - ang1_out) < 1e-10 &&
                     std::abs(angle2 - ang2_out) < 1e-10 &&
                     std::abs(angle3 - ang3_out) < 1e-10;
    
    printf("输入输出比较 (欧拉角):\n");
    printf("  angle1: 原始=%.6f, 转换后=%.6f, 一致=%s\n", angle1, ang1_out, 
           std::abs(angle1 - ang1_out) < 1e-10 ? "是" : "否");
    printf("  angle2: 原始=%.6f, 转换后=%.6f, 一致=%s\n", angle2, ang2_out,
           std::abs(angle2 - ang2_out) < 1e-10 ? "是" : "否");
    printf("  angle3: 原始=%.6f, 转换后=%.6f, 一致=%s\n", angle3, ang3_out,
           std::abs(angle3 - ang3_out) < 1e-10 ? "是" : "否");
    printf("欧拉角转换验证: %s\n", eul_match ? "通过" : "失败");
    
    // 示例：旋转矩阵转四元数
    Quaternion q;
    m2q(rotation, q);
    
    printf("\n四元数转换:\n");
    printf("四元数: w=%.6f, x=%.6f, y=%.6f, z=%.6f\n", q.w(), q.x(), q.y(), q.z());
    
    // 示例：四元数转旋转矩阵
    Matrix3d rotation2;
    q2m(q, rotation2);
    
    printf("旋转矩阵:\n");
    printf("  原始:\n");
    for (int i = 0; i < 3; i++) {
        printf("    [%8.4f %8.4f %8.4f]\n", rotation(i,0), rotation(i,1), rotation(i,2));
    }
    printf("  四元数->矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("    [%8.4f %8.4f %8.4f]\n", rotation2(i,0), rotation2(i,1), rotation2(i,2));
    }
    
    // 验证矩阵一致性
    bool isEqual = true;
    printf("\n输入输出比较 (旋转矩阵):\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            double diff = std::abs(rotation(i,j) - rotation2(i,j));
            if (diff > 1e-10) {
                isEqual = false;
            }
            printf("  [%d,%d]: 原始=%.6f, 转换后=%.6f, 差异=%.2e\n", 
                   i, j, rotation(i,j), rotation2(i,j), diff);
        }
    }
    printf("矩阵-四元数转换验证: %s\n", isEqual ? "通过" : "失败");
    
    bool all_pass = eul_match && isEqual;
    printf("\n整体验证: %s\n", all_pass ? "通过" : "失败");
    
    return all_pass ? 0 : 1;
}
