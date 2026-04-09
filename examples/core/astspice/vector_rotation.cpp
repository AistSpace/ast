#include "AstSPICE/SpiceZpr.hpp"
#include <cstdio>
#include <cmath>

AST_USING_NAMESPACE

int main()
{
    // 示例：绕指定轴旋转向量
    Vector3d v = {1, 0, 0};    // 初始向量沿X轴
    Vector3d axis = {0, 0, 1}; // 绕Z轴旋转
    double theta = AST_PI / 4; // 45度
    
    Vector3d result;
    vrotv(v, axis, theta, result);
    
    printf("向量旋转示例:\n");
    printf("输入向量: (%.6f, %.6f, %.6f)\n", v[0], v[1], v[2]);
    printf("旋转轴: (%.6f, %.6f, %.6f)\n", axis[0], axis[1], axis[2]);
    printf("旋转角度: %.6f 弧度 (%.2f 度)\n", theta, theta * 180 / AST_PI);
    printf("结果向量: (%.6f, %.6f, %.6f)\n", result[0], result[1], result[2]);
    
    // 验证：绕Z轴旋转45度，X轴向量应该变成(cos45, sin45, 0)
    double expected_x = std::cos(theta);
    double expected_y = std::sin(theta);
    double expected_z = 0.0;
    
    printf("\n结果验证:\n");
    printf("  期望X: %.6f, 实际X: %.6f, 差异: %.2e\n", expected_x, result[0], std::abs(result[0] - expected_x));
    printf("  期望Y: %.6f, 实际Y: %.6f, 差异: %.2e\n", expected_y, result[1], std::abs(result[1] - expected_y));
    printf("  期望Z: %.6f, 实际Z: %.6f, 差异: %.2e\n", expected_z, result[2], std::abs(result[2] - expected_z));
    
    bool isCorrect = std::abs(result[0] - expected_x) < 1e-10 &&
                     std::abs(result[1] - expected_y) < 1e-10 &&
                     std::abs(result[2] - expected_z) < 1e-10;
    
    printf("向量旋转验证: %s\n", isCorrect ? "通过" : "失败");
    
    // 示例：矩阵乘法
    Matrix3d m1, m2, mout;
    m1 = Matrix3d::Identity();
    m1(0, 1) = 0.5;
    m1(1, 0) = -0.5;
    
    m2 = Matrix3d::Identity();
    m2(0, 2) = 0.3;
    m2(2, 0) = -0.3;
    
    mxm(m1, m2, mout);
    
    printf("\n矩阵乘法示例:\n");
    printf("矩阵1:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%8.4f %8.4f %8.4f]\n", m1(i,0), m1(i,1), m1(i,2));
    }
    printf("矩阵2:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%8.4f %8.4f %8.4f]\n", m2(i,0), m2(i,1), m2(i,2));
    }
    printf("结果:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%8.4f %8.4f %8.4f]\n", mout(i,0), mout(i,1), mout(i,2));
    }
    
    // 验证矩阵乘法结果
    // m1 * m2 = [[1, 0.5, 0.3], [-0.5*0.3, 1, -0.5], [0.3, 0, 1]]
    // 详细计算:
    // [0,0]: 1*1 + 0.5*0 + 0*0.3 = 1
    // [0,1]: 1*0 + 0.5*1 + 0*0 = 0.5
    // [0,2]: 1*0.3 + 0.5*0 + 0*1 = 0.3
    // [1,0]: -0.5*1 + 1*0 + 0*0.3 = -0.5
    // [1,1]: -0.5*0 + 1*1 + 0*0 = 1
    // [1,2]: -0.5*0.3 + 1*0 + 0*1 = -0.15
    // [2,0]: 0.3*1 + 0*0 + 1*0.3 = 0.6
    // [2,1]: 0.3*0 + 0*1 + 1*0 = 0
    // [2,2]: 0.3*0.3 + 0*0 + 1*1 = 1.09
    
    Matrix3d expected;
    expected << 1.0, 0.5, 0.3,
                -0.5, 1.0, -0.15,
                0.6, 0.0, 1.09;
    
    printf("\n结果验证:\n");
    bool matrix_correct = true;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            double diff = std::abs(mout(i,j) - expected(i,j));
            if (diff > 1e-10) {
                matrix_correct = false;
            }
            printf("  [%d,%d]: 期望=%.4f, 实际=%.4f, 差异=%.2e %s\n", 
                   i, j, expected(i,j), mout(i,j), diff, 
                   diff < 1e-10 ? "✓" : "✗");
        }
    }
    
    printf("矩阵乘法验证: %s\n", matrix_correct ? "通过" : "失败");
    
    bool all_pass = isCorrect && matrix_correct;
    printf("\n整体验证: %s\n", all_pass ? "通过" : "失败");
    
    return all_pass ? 0 : 1;
}
