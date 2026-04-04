#include "MotionTwoBody.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class MotionTwoBody::staticType;

static bool MotionTwoBody_ClassInited = (MotionTwoBody::ClassInit(&MotionTwoBody::staticType), true);

void MotionTwoBody::ClassInit(Class* cls)
{

    cls->setName("MotionTwoBody");
    cls->addToRegistry();
    cls->setParent<MotionOrbitDynamics>();
    cls->setConstructor<MotionTwoBody>();

}

AST_NAMESPACE_END