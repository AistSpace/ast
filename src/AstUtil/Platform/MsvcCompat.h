///
/// @file      MsvcCompat.h
/// @brief     MSVC 新版本 STL 的兼容垫片
/// @details   Qt 5.15.2 的 qcompilerdetection.h 在 MSVC 下无条件把
///            QT_MAKE_CHECKED_ARRAY_ITERATOR / QT_MAKE_UNCHECKED_ARRAY_ITERATOR 展开成
///            stdext::make_checked_array_iterator / stdext::make_unchecked_array_iterator。
///            这两个非标准扩展自 MSVC 14.51（VS2026，_MSC_VER 1951）起已从 STL 移除，
///            于是任何实例化了 QVector<T>::operator== 的编译单元都会报 C2653/C3861
///            这里补上同名工厂函数。未启用检查迭代器时它们本来就等价于返回原始指针
///            （Qt 自己在非 MSVC 分支的回退宏就是 (x)），所以直接返回指针即可
/// @author    axel
/// @date      2026-10-22
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

#pragma once

#ifdef __cplusplus
#if defined(_MSC_VER) && _MSC_VER >= 1951

#include <cstddef>

namespace stdext {

template <class T>
inline T* make_checked_array_iterator(T* ptr, std::size_t /*size*/)
{
    return ptr;
}

template <class T>
inline T* make_unchecked_array_iterator(T* ptr)
{
    return ptr;
}

} // namespace stdext

#endif
#endif // __cplusplus
