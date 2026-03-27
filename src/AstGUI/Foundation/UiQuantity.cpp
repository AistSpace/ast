///
/// @file      UiQuantity.cpp
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

#include "UiQuantity.hpp"
#include "AstUtil/Unit.hpp"

AST_NAMESPACE_BEGIN

UiQuantity::UiQuantity(QWidget* parent)
    : QWidget(parent)
    , layout_(new QHBoxLayout(this))
    , valueEdit_(new QLineEdit(this))
    , unitComboBox_(new QComboBox(this))
{
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->addWidget(valueEdit_);
    layout_->addWidget(unitComboBox_);
    
    connect(valueEdit_, &QLineEdit::textChanged, this, &UiQuantity::onValueChanged);
    connect(unitComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UiQuantity::onUnitChanged);
    
    // 默认添加一些常用单位
    unitComboBox_->addItem("sec", QVariant::fromValue(units::sec));
    unitComboBox_->addItem("s", QVariant::fromValue(units::s));
    unitComboBox_->addItem("ms", QVariant::fromValue(units::ms));
    unitComboBox_->addItem("min", QVariant::fromValue(units::min));
    unitComboBox_->addItem("hour", QVariant::fromValue(units::hour));
    unitComboBox_->addItem("h", QVariant::fromValue(units::h));
    unitComboBox_->addItem("day", QVariant::fromValue(units::day));
    
    unitComboBox_->addItem("mm", QVariant::fromValue(units::mm));
    unitComboBox_->addItem("cm", QVariant::fromValue(units::cm));
    unitComboBox_->addItem("dm", QVariant::fromValue(units::dm));
    unitComboBox_->addItem("m", QVariant::fromValue(units::m));
    unitComboBox_->addItem("km", QVariant::fromValue(units::km));
    
    unitComboBox_->addItem("g", QVariant::fromValue(units::g));
    unitComboBox_->addItem("kg", QVariant::fromValue(units::kg));
    unitComboBox_->addItem("mg", QVariant::fromValue(units::mg));
    unitComboBox_->addItem("lb", QVariant::fromValue(units::lb));
    
    unitComboBox_->addItem("deg", QVariant::fromValue(units::deg));
    unitComboBox_->addItem("rad", QVariant::fromValue(units::rad));
    unitComboBox_->addItem("arcsec", QVariant::fromValue(units::arcsec));
}

void UiQuantity::setQuantity(const Quantity& quantity)
{
    currentQuantity_ = quantity;
    
    // 更新数值输入框
    valueEdit_->setText(QString::number(quantity.value()));
    
    // 更新单位下拉框
    for (int i = 0; i < unitComboBox_->count(); ++i) {
        Unit unit = unitComboBox_->itemData(i).value<Unit>();
        if (unit == quantity.unit()) {
            unitComboBox_->setCurrentIndex(i);
            break;
        }
    }
}

Quantity UiQuantity::quantity() const
{
    return currentQuantity_;
}

void UiQuantity::onValueChanged()
{
    bool ok;
    double value = valueEdit_->text().toDouble(&ok);
    if (ok) {
        Unit unit = unitComboBox_->currentData().value<Unit>();
        currentQuantity_ = Quantity(value, unit);
        emit quantityChanged(currentQuantity_);
    }
}

void UiQuantity::onUnitChanged(int index)
{
    if (index >= 0) {
        Unit newUnit = unitComboBox_->itemData(index).value<Unit>();
        // 转换数值到新单位
        double oldValue = currentQuantity_.value();
        Unit oldUnit = currentQuantity_.unit();
        
        double newValue = oldValue;
        if (oldUnit.isValid()) {
            // 先转换到SI单位，再转换到新单位
            double siValue = oldUnit.toSI(oldValue);
            newValue = newUnit.fromSI(siValue);
        }
        
        // 更新数值输入框
        valueEdit_->setText(QString::number(newValue));
        
        // 更新当前数量值
        currentQuantity_ = Quantity(newValue, newUnit);
        emit quantityChanged(currentQuantity_);
    }
}

AST_NAMESPACE_END