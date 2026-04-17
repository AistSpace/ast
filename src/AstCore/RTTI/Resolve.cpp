///
/// @file      Resolve.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-17
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

#include "Resolve.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstCore/FrameAssembly.hpp"

AST_NAMESPACE_BEGIN

Frame *aObject_GetFrame(Object* obj, StringView frameName)
{
    auto frame = aFindChild(obj, Frame::getStaticType(), frameName);
    if(frame)
        return (Frame*)frame;
    if(obj->getType() == CelestialBody::getStaticType())
    {
        CelestialBody* body = (CelestialBody*)obj;
        HFrame frame;
        if(frameName == "Inertial")
        {
            frame = body->makeFrameInertial();
        }
        else if(frameName == "ICRF")
        {
            frame = body->makeFrameICRF();
        }
        else if(frameName == "TOD")
        {
            frame = body->makeFrameTOD();
        }
        if(frame)
        {
            frame->setName(frameName);
            return frame.take();
        }
    }
    return nullptr;
}



Body *aResolveBody(StringView name)
{
    auto iter = name.find("/");
    if(iter == StringView::npos)
    {
        return nullptr;
    }
    auto prefix = name.substr(0, iter);
    if(prefix!="CentralBody")
        return nullptr;
    auto bodyName = name.substr(iter + 1);
    return aGetBody(bodyName);    
}

Frame *aResolveFrame(StringView name)
{
    auto iter = name.find(' ');
    if(iter == StringView::npos)
        return nullptr;
    StringView objpath = name.substr(0, iter);
    auto obj = aResolveObject(objpath);
    if(obj == nullptr)
        return nullptr;
    StringView frameName = name.substr(iter + 1);
    return aObject_GetFrame(obj, frameName);
}

Object *aResolveObject(StringView path)
{
    auto iter = path.find("/");
    if(iter == StringView::npos)
    {
        return nullptr;
    }
    auto className = path.substr(0, iter);
    auto objName = path.substr(iter + 1);
    if(className=="CentralBody")
    {
        return aGetBody(objName);
    }
    return nullptr;    
}

AST_NAMESPACE_END

