#include "StateCartesian.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StateCartesian::staticType;

static bool StateCartesian_ClassInited = (StateCartesian::ClassInit(&StateCartesian::staticType), true);

void StateCartesian::ClassInit(Class* cls)
{

    cls->setName("StateCartesian");
    cls->addToRegistry();
    cls->setParent<State>();
    cls->setConstructor<StateCartesian>();

    cls->addProperty("x", aNewPropertyDouble<StateCartesian, &StateCartesian::x, &StateCartesian::setX>());
    cls->addProperty("y", aNewPropertyDouble<StateCartesian, &StateCartesian::y, &StateCartesian::setY>());
    cls->addProperty("z", aNewPropertyDouble<StateCartesian, &StateCartesian::z, &StateCartesian::setZ>());
    cls->addProperty("vx", aNewPropertyDouble<StateCartesian, &StateCartesian::vx, &StateCartesian::setVx>());
    cls->addProperty("vy", aNewPropertyDouble<StateCartesian, &StateCartesian::vy, &StateCartesian::setVy>());
    cls->addProperty("vz", aNewPropertyDouble<StateCartesian, &StateCartesian::vz, &StateCartesian::setVz>());
}

AST_NAMESPACE_END