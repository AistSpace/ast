#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/TimePoint.hpp"
#include <stdio.h>

AST_USING_NAMESPACE

int main()
{
    printf("轴系层级操作示例:\n\n");
    
    // 获取各种轴系
    Axes* root = aAxesRoot();
    Axes* icrf = aAxesICRF();
    Axes* j2000 = aAxesJ2000();
    Axes* mod = aAxesMOD();
    Axes* tod = aAxesTOD();
    Axes* ecf = aAxesECF();
    
    // 1. 获取父轴系
    printf("--- 获取父轴系 ---\n");
    printf("ICRF的父轴系: %p (应为Root: %p)\n", 
           (void*)icrf->getParent(), (void*)root);
    printf("ECF的父轴系: %p (应为TOD: %p)\n", 
           (void*)ecf->getParent(), (void*)tod);
    
    // 2. 获取轴系深度
    printf("\n--- 轴系深度 ---\n");
    printf("Root深度: %d\n", root->getDepth());
    printf("ICRF深度: %d\n", icrf->getDepth());
    printf("J2000深度: %d\n", j2000->getDepth());
    printf("MOD深度: %d\n", mod->getDepth());
    printf("TOD深度: %d\n", tod->getDepth());
    printf("ECF深度: %d\n", ecf->getDepth());
    
    // 3. 获取祖先轴系
    printf("\n--- 获取祖先轴系 ---\n");
    printf("ECF的第0层祖先(自身): %p (应为ECF: %p)\n", 
           (void*)ecf->getAncestor(0), (void*)ecf);
    printf("ECF的第1层祖先: %p (应为TOD: %p)\n", 
           (void*)ecf->getAncestor(1), (void*)tod);
    printf("ECF的第2层祖先: %p (应为MOD: %p)\n", 
           (void*)ecf->getAncestor(2), (void*)mod);
    printf("ECF的第3层祖先: %p (应为J2000: %p)\n", 
           (void*)ecf->getAncestor(3), (void*)j2000);
    printf("ECF的第4层祖先: %p (应为ICRF: %p)\n", 
           (void*)ecf->getAncestor(4), (void*)icrf);
    printf("ECF的第5层祖先: %p (应为Root: %p)\n", 
           (void*)ecf->getAncestor(5), (void*)root);
    
    // 4. 层级遍历
    printf("\n--- 从ECF向上遍历到Root ---\n");
    Axes* current = ecf;
    int level = 0;
    while (current != nullptr) {
        printf("Level %d: %p\n", level, (void*)current);
        current = current->getParent();
        level++;
    }
    
    return 0;
}
