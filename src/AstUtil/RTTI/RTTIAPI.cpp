///
/// @file      RTTIAPI.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-02
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "RTTIAPI.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/ClassRegistry.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectManager.hpp"


AST_NAMESPACE_BEGIN

void aGetAllClassNames(std::vector<std::string> &names)
{
    ClassRegistry::Instance()->getAllClassNames(names);
}

Class *aGetClass(StringView name)
{
    return ClassRegistry::Instance()->getClass(name);
}

void aRegisterClass(Class *cls)
{
    ClassRegistry::Instance()->registerClass(cls);
}

Object *aGetClassDefaultObject(StringView name)
{
    Class *cls = aGetClass(name);
    return aGetClassDefaultObject(cls);
}

Object *aGetClassDefaultObject(Class *cls)
{
    if(!cls)
        return nullptr;
    return cls->getDefaultObject();
}

Object *aNewObject(StringView name, Object* parentScope)
{
    Class *cls = aGetClass(name);
    if(!cls)
        return nullptr;
    return cls->NewObject(parentScope);
}

void aDeleteObject(Object *obj)
{
    obj->destruct();
}

SharedPtr<Object> aMakeObject(StringView name, Object* parentScope)
{
    return aNewObject(name, parentScope);
}

Object *aGetObject(uint32_t id)
{
    return ObjectManager::CurrentInstance().getObject(id);
}

uint32_t aAddObject(Object *object)
{
    return ObjectManager::CurrentInstance().addObject(object);
}

errc_t aSetParentScope(Object *obj, Object *parentScope)
{
    return ObjectManager::CurrentInstance().setParentScope(obj, parentScope);
}

Object *aGetParentScope(Object *obj)
{
    return ObjectManager::CurrentInstance().getParentScope(obj);
}

AST_NAMESPACE_END

