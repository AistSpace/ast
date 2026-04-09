#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/Matrix.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

int main()
{
    // 创建时间点
    TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 12, 0, 0);
    
    printf("获取旋转变换示例:\n\n");
    
    // 获取两个轴系
    Axes* j2000 = aAxesJ2000();
    Axes* ecf = aAxesECF();
    
    // 方式1: 使用全局函数
    printf("--- 使用全局函数 aAxesTransform ---\n");
    Rotation rotation1;
    errc_t err = aAxesTransform(j2000, ecf, tp, rotation1);
    if (err == eNoError) {
        Matrix3d mat1 = rotation1.getMatrix();
        printf("J2000到ECF转换矩阵:\n");
        for (int i = 0; i < 3; i++) {
            printf("  [%.8f %.8f %.8f]\n", mat1(i,0), mat1(i,1), mat1(i,2));
        }
    }
    
    // 方式2: 使用成员函数 getTransformTo
    printf("\n--- 使用成员函数 getTransformTo ---\n");
    Rotation rotation2;
    j2000->getTransformTo(ecf, tp, rotation2);
    
    Matrix3d mat2 = rotation2.getMatrix();
    printf("J2000到ECF转换矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.8f %.8f %.8f]\n", mat2(i,0), mat2(i,1), mat2(i,2));
    }
    
    // 方式3: 使用成员函数 getTransformFrom
    printf("\n--- 使用成员函数 getTransformFrom ---\n");
    Rotation rotation3;
    ecf->getTransformFrom(j2000, tp, rotation3);
    
    Matrix3d mat3 = rotation3.getMatrix();
    printf("J2000到ECF转换矩阵:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%.8f %.8f %.8f]\n", mat3(i,0), mat3(i,1), mat3(i,2));
    }
    
    return 0;
}
