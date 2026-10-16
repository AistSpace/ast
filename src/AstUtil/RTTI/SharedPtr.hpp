///
/// @file      SharedPtr.hpp
/// @brief     共享指针
/// @details   与std::shared_ptr功能类似
/// @author    axel
/// @date      19.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
 
AST_NAMESPACE_BEGIN

/// @brief 共享指针
/// @tparam _Object 指向的对象类型
/// @details   与std::shared_ptr功能类似，但是不支持自定义删除器
/// @ingroup RTTI
template<typename _Object>
class SharedPtr
{
public:
    SharedPtr() noexcept
        :object_{ nullptr }
    {}
    SharedPtr(std::nullptr_t) noexcept
        :object_{ nullptr }
    {
    }
    SharedPtr(_Object* obj) noexcept
        :object_{ obj }
    {
        _incRef();
    }
    SharedPtr(const SharedPtr& ptr) noexcept
        :SharedPtr{ptr.get()}
    {

    }
    ~SharedPtr() noexcept
    {
        _decRef();
    }
    SharedPtr& operator=(const SharedPtr& ptr) noexcept
    {
        return this->operator=(ptr.get());
    }

    SharedPtr& operator=(_Object* obj) noexcept
    {
        if(obj != object_){ 
            if(obj)
                obj->incRef();
            _decRef();
            object_ = obj;
        }
        return *this;
    }
    SharedPtr& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }
    // 这里还是设置为支持隐式转换，因为在很多情况下，我们需要将SharedPtr转换为_Object*
    // explicit 
    operator _Object*() const noexcept
    {
        return object_;
    }
    explicit operator bool() const noexcept
    {
        return object_ != nullptr;
    }
    _Object* operator->() const noexcept
    {
        return object_;
    }
    _Object* get() const noexcept
    {
        return object_;
    }
    _Object* take() noexcept
    {
        _Object* obj = object_;
        if(obj){
            obj->decRefNoDelete();
            object_ = nullptr;
        }
        return obj;
    }
    void reset() noexcept
    {
        _decRef();
        object_ = nullptr;
    }
protected:
    void _incRef() noexcept
    {
        if(object_)
            object_->incRef();
    }
    void _decRef() noexcept
    {
        if(object_)
            object_->decRef();
    }
protected:
    _Object* object_{nullptr};
};


 
AST_NAMESPACE_END
 
