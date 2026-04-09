#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/AxesFrozen.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/Matrix.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

int main()
{
    // 创建冻结时间和求值时间
    TimePoint freezeTime = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    TimePoint evalTime = TimePoint::FromUTC(2026, 6, 15, 12, 0, 0);  // 6个月后
    
    printf("AxesFrozen冻结轴系示例:\n\n");
    
    // 获取ECF轴系（使用智能指针）
    HAxes ecf = aAxesECF();
    
    // 创建冻结轴系：使用MakeShared避免内存泄漏
    // 在freezeTime时刻冻结ECF轴系
    HAxesFrozen frozenAxes = AxesFrozen::MakeShared(ecf.get(), freezeTime, aAxesICRF());
    
    printf("冻结时间: 2026-01-01 00:00:00 UTC\n");
    printf("求值时间: 2026-06-15 12:00:00 UTC\n");
    
    // 获取冻结轴系到ICRF的变换（在冻结时刻的值）
    Rotation rotation;
    errc_t err = frozenAxes->getTransformTo(aAxesICRF(), freezeTime, rotation);
    if (err != eNoError) {
        printf("错误: 获取变换失败，错误码: %d\n", (int)err);
        return 1;
    }
    
    printf("\n在冻结时刻的变换矩阵:\n");
    Matrix3d matrix1 = rotation.getMatrix();
    for (int i = 0; i < 3; i++) {
        printf("  [%.8f %.8f %.8f]\n", matrix1(i,0), matrix1(i,1), matrix1(i,2));
    }
    
    // 在其他时刻获取变换（仍然是冻结时的值）
    Rotation rotation2;
    err = frozenAxes->getTransformTo(aAxesICRF(), evalTime, rotation2);
    if (err != eNoError) {
        printf("错误: 获取变换失败，错误码: %d\n", (int)err);
        return 1;
    }
    
    printf("\n在求值时刻的变换矩阵（应该是相同的）:\n");
    Matrix3d matrix2 = rotation2.getMatrix();
    for (int i = 0; i < 3; i++) {
        printf("  [%.8f %.8f %.8f]\n", matrix2(i,0), matrix2(i,1), matrix2(i,2));
    }
    
    // 比较两次结果是否相同
    bool isSame = (matrix1 - matrix2).norm() < 1e-10;
    printf("\n两次结果是否相同: %s\n", isSame ? "是" : "否");
    
    // 冻结轴系超出作用域时自动释放内存，无需手动delete
    return 0;
}
