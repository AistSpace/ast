///
/// @file      UiQuantity.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-26
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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

#include "AstGlobal.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QMetaType>
#include "AstUtil/Quantity.hpp"
#include "AstUtil/Unit.hpp"

Q_DECLARE_METATYPE(ast::Unit)

AST_NAMESPACE_BEGIN

/*!
    @addtogroup AstGUI
    @{
*/

class AST_GUI_API UiQuantity: public QWidget
{
    Q_OBJECT

public:
    UiQuantity(QWidget* parent = nullptr);
    
    /// @brief 设置数量值
    /// @param quantity 数量值
    void setQuantity(const Quantity& quantity);
    
    /// @brief 获取数量值
    /// @return 数量值
    Quantity quantity() const;

signals:
    /// @brief 数量值变化信号
    void quantityChanged(const Quantity& quantity);

private slots:
    /// @brief 处理数值变化
    void onValueChanged();
    
    /// @brief 处理单位变化
    void onUnitChanged(int index);

private:
    QHBoxLayout* layout_;
    QLineEdit* valueEdit_;
    QComboBox* unitComboBox_;
    Quantity currentQuantity_;
};

/*! @} */

AST_NAMESPACE_END