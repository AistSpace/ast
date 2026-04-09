#include "AstSPICE/SpiceZpr.hpp"

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
    
    return 0;
}
