#include "StateCartesian.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StateCartesian::staticType;

static bool StateCartesian_ClassInited = (StateCartesian::ClassInit(&StateCartesian::staticType), true);

void StateCartesian::ClassInit(Class* cls)
{

    cls->addProperty("X", aNewPropertyDouble<StateCartesian, &StateCartesian::getX, &StateCartesian::setX>());
    cls->addProperty("Y", aNewPropertyDouble<StateCartesian, &StateCartesian::getY, &StateCartesian::setY>());
    cls->addProperty("Z", aNewPropertyDouble<StateCartesian, &StateCartesian::getZ, &StateCartesian::setZ>());
    cls->addProperty("Vx", aNewPropertyDouble<StateCartesian, &StateCartesian::getVx, &StateCartesian::setVx>());
    cls->addProperty("Vy", aNewPropertyDouble<StateCartesian, &StateCartesian::getVy, &StateCartesian::setVy>());
    cls->addProperty("Vz", aNewPropertyDouble<StateCartesian, &StateCartesian::getVz, &StateCartesian::setVz>());
}

AST_NAMESPACE_END