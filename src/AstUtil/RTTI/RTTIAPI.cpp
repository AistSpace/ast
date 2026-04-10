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
#include "AstUtil/Platform/ColoredPrint.hpp"
#include <cstdio>


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

Object *aFindChild(Object *parentScope, Class *cls, StringView name)
{
    if(!parentScope)
        return nullptr;
    
    // 获取父对象对应的对象节点
    auto& objectManager = ObjectManager::CurrentInstance();
    auto parentNode = objectManager.getObjectNode(parentScope);
    if(!parentNode)
        return nullptr;
    
    // 遍历父对象的子节点
    for(auto childNode : parentNode->getChildren())
    {
        if(childNode->expired())
            continue;
        
        auto childObject = childNode->getObject();
        if(!childObject)
            continue;
        
        // 检查类型是否匹配
        if(cls)
        {
            bool typeMatch = false;
            auto childType = childObject->getType();
            while(childType)
            {
                if(childType == cls)
                {
                    typeMatch = true;
                    break;
                }
                childType = childType->getParent();
            }
            if(!typeMatch)
                continue;
        }
        
        // 检查名称是否匹配
        if(name.data())
        {
            if(name != childObject->getName())
                continue;
        }
        
        return childObject;
    }
    
    return nullptr;
}

errc_t aFindChildren(Object *parentScope, Class *cls, StringView name, std::vector<Object*> &children)
{
    if(!parentScope)
        return -1;
    
    children.clear();
    
    // 获取父对象对应的对象节点
    auto& objectManager = ObjectManager::CurrentInstance();
    auto parentNode = objectManager.getObjectNode(parentScope);
    if(!parentNode)
        return -1;
    auto& childrenNodes = parentNode->getChildren();
    children.reserve(childrenNodes.size());
    
    // 遍历父对象的子节点
    for(auto childNode : childrenNodes)
    {
        if(childNode->expired())
            continue;
        
        auto childObject = childNode->getObject();
        if(!childObject)
            continue;
        
        // 检查类型是否匹配
        if(cls)
        {
            bool typeMatch = false;
            auto childType = childObject->getType();
            while(childType)
            {
                if(childType == cls)
                {
                    typeMatch = true;
                    break;
                }
                childType = childType->getParent();
            }
            if(!typeMatch)
                continue;
        }
        
        // 检查名称是否匹配
        if(name.data())
        {
            if(name != childObject->getName())
                continue;
        }
        
        children.push_back(childObject);
    }
    
    return 0;
}

std::vector<Object*> aFindChildren(Object* parentScope, Class* cls, StringView name)
{
    std::vector<Object*> children;
    aFindChildren(parentScope, cls, name, children);
    return children;
}

void aPrintObjectTree(Object* root, int indent)
{
    aPrintObjectTree(root, indent, PrintObjectTreeConfig());
}

void aPrintObjectTree(Object* root, int indent, const PrintObjectTreeConfig& config)
{
    if (!root) {
        return;
    }

    // 打印缩进
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    // 打印对象信息
    const std::string& name = root->getName();
    Class* type = root->getType();
    uint32_t id = root->getID();
    uint32_t refCount = root->refCount();
    uint32_t weakRefCount = root->weakRefCount();

    // 使用不同颜色和符号来区分字段
    cprintf(eGreen, "%s", name.c_str());
    cprintf(eLightGray, "#%u", id);
    cprintf(eCyan, " <%s>", type ? type->name().c_str() : "Unknown");
    
    // 根据配置决定是否打印引用计数
    if (config.printRefCount) {
        cprintf(eLightPink, " R:%u", refCount);
    }
    if (config.printWeakRefCount) {
        cprintf(eLightBlue, " W:%u", weakRefCount);
    }
    
    printf("\n");

    // 打印子对象
    std::vector<Object*> children = aFindChildren(root);
    for (Object* child : children) {
        aPrintObjectTree(child, indent + 1, config);
    }
}

AST_NAMESPACE_END