#include "AstSPICE/SpiceZpr.hpp"
#include <cstdio>
#include <cmath>

AST_USING_NAMESPACE

int main()
{
    // 绕Z轴旋转45度
    Vector3d axis = {0, 0, 1};
    double angle = AST_PI / 4;  // 45度
    Matrix3d rotation;
    
    axisar(axis, angle, rotation);
    
    // 验证结果：旋转矩阵第一列应该是(cos, sin, 0)
    double expected_x = std::cos(angle);
    double expected_y = std::sin(angle);
    
    printf("axisar旋转矩阵示例:\n");
    printf("旋转轴: (%.1f, %.1f, %.1f), 角度: %.2f rad\n", axis[0], axis[1], axis[2], angle);
    printf("旋转矩阵第一列: (%.6f, %.6f, %.6f)\n", rotation(0,0), rotation(1,0), rotation(2,0));
    
    // 实际使用expected_x和expected_y验证结果
    bool isCorrect = std::abs(rotation(0,0) - expected_x) < 1e-10 &&
                     std::abs(rotation(1,0) - expected_y) < 1e-10 &&
                     std::abs(rotation(2,0)) < 1e-10;
    
    printf("验证: expected_x=%.6f, actual=%.6f, match=%s\n", expected_x, rotation(0,0), 
           std::abs(rotation(0,0) - expected_x) < 1e-10 ? "yes" : "no");
    printf("验证: expected_y=%.6f, actual=%.6f, match=%s\n", expected_y, rotation(1,0),
           std::abs(rotation(1,0) - expected_y) < 1e-10 ? "yes" : "no");
    printf("整体验证: %s\n", isCorrect ? "通过" : "失败");
    
    return isCorrect ? 0 : 1;
}
