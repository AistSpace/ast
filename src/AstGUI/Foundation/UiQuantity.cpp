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
#include "AstUtil/QuantityParser.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

UiQuantity::UiQuantity(QWidget* parent)
    : QLineEdit(parent)
{
    setToolTipDuration(0);
    connect(this, &QLineEdit::editingFinished, this, &UiQuantity::updateQuantity);
}

void UiQuantity::setQuantity(const Quantity& quantity)
{
    currentQuantity_ = quantity;
    setText(QString::fromUtf8(currentQuantity_.toString().c_str()));
}

Quantity UiQuantity::getQuantity() const
{
    return currentQuantity_;
}

double UiQuantity::getMagnitude() const
{
    return currentQuantity_.magnitude();
}

void UiQuantity::setMagnitude(double value)
{
    currentQuantity_.setMagnitude(value);
    this->setQuantity(currentQuantity_);
}

Unit UiQuantity::getUnit() const
{
    return currentQuantity_.unit();
}

void UiQuantity::setUnit(const Unit& unit)
{
    currentQuantity_.setUnit(unit);
    this->setQuantity(currentQuantity_);
}

void UiQuantity::changeUnit(const Unit& unit)
{
    currentQuantity_.changeUnit(unit);
    this->setQuantity(currentQuantity_);
}

double UiQuantity::getValueSI() const
{
    return currentQuantity_.getValueSI();
}

void UiQuantity::setValueSI(double value)
{
    currentQuantity_.setValueSI(value);
    this->setQuantity(currentQuantity_);
}

double UiQuantity::getValueInUnit(const Unit& unit) const
{
    return currentQuantity_.getValueInUnit(unit);
}

void UiQuantity::setValueInUnit(double value, const Unit& unit)
{
    currentQuantity_.setValueInUnit(value, unit);
    this->setQuantity(currentQuantity_);
}

void UiQuantity::setValueUnit(double value, const Unit& unit)
{
    currentQuantity_.setValueUnit(value, unit);
    this->setQuantity(currentQuantity_);
}

void UiQuantity::updateQuantity()
{
    QString text = this->text();
    errc_t rc = aQuantityParse(text.toUtf8().data(), currentQuantity_);
    if(rc){
        // 显示错误提示
        aError("failed to parse quantity: %s", text.toUtf8().data());
        setStyleSheet("background-color: #FFCCCC;");
        setToolTip(tr("quantity format error, please input correct quantity format"));
    }else{
        // 解析成功，恢复默认样式
        setStyleSheet("");
        setToolTip("");
        emit quantityChanged(currentQuantity_);
    }
}


AST_NAMESPACE_END