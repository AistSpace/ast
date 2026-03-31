///
/// @file      UiMotionTwoBody.cpp
/// @brief     二体运动模型UI组件
/// @author    axel
/// @date      2026-03-28
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

#include "UiMotionTwoBody.hpp"
#include "AstSim/MotionTwoBody.hpp"
#include "AstGUI/UiStateCartesian.hpp"
#include "AstGUI/UiTimeInterval.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstUtil/Unit.hpp"
#include "AstUtil/Quantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>

AST_NAMESPACE_BEGIN

using namespace units;

UiMotionTwoBody::UiMotionTwoBody(QWidget *parent)
    : UiMotionProfile(parent)
{
    // 创建主布局
    mainLayout_ = new QVBoxLayout(this);
    
    // 创建网格布局，分为左右两列
    gridLayout_ = new QGridLayout();
    
    // 创建左侧布局
    leftLayout_ = new QVBoxLayout();
    
    // 时间区间
    timeIntervalLayout_ = new QHBoxLayout();
    timeIntervalLabel_ = new QLabel(tr("时间区间"), this);
    timeIntervalEdit_ = new UiTimeInterval(this);
    timeIntervalLayout_->addWidget(timeIntervalLabel_);
    timeIntervalLayout_->addWidget(timeIntervalEdit_);
    leftLayout_->addLayout(timeIntervalLayout_);
    
    // 仿真步长
    stepSizeLayout_ = new QHBoxLayout();
    stepSizeLabel_ = new QLabel(tr("仿真步长"), this);
    stepSizeEdit_ = new UiQuantity(this);
    stepSizeEdit_->setQuantity(Quantity(60, s));
    stepSizeLayout_->addWidget(stepSizeLabel_);
    stepSizeLayout_->addWidget(stepSizeEdit_);
    leftLayout_->addLayout(stepSizeLayout_);
    
    // 预报系
    propagationFrameLayout_ = new QHBoxLayout();
    propagationFrameLabel_ = new QLabel(tr("预报系"), this);
    propagationFrameCombo_ = new QComboBox(this);
    propagationFrameCombo_->addItem(tr("ICRF"));
    propagationFrameLayout_->addWidget(propagationFrameLabel_);
    propagationFrameLayout_->addWidget(propagationFrameCombo_);
    leftLayout_->addLayout(propagationFrameLayout_);
    
    // 坐标类型
    coordTypeLayout_ = new QHBoxLayout();
    coordTypeLabel_ = new QLabel(tr("坐标类型"), this);
    coordTypeCombo_ = new QComboBox(this);
    coordTypeCombo_->addItem(tr("位置速度"));
    coordTypeLayout_->addWidget(coordTypeLabel_);
    coordTypeLayout_->addWidget(coordTypeCombo_);
    leftLayout_->addLayout(coordTypeLayout_);
    
    // 创建右侧布局
    rightLayout_ = new QVBoxLayout();
    
    // 使用UiStateCartesian组件
    stateCartesianEdit_ = new UiStateCartesian(this);
    rightLayout_->addWidget(stateCartesianEdit_);
    
    // 将左右布局添加到网格布局
    gridLayout_->addLayout(leftLayout_, 0, 0);
    gridLayout_->addLayout(rightLayout_, 0, 1);
    
    // 设置列宽比例，使左右两列更加协调
    // gridLayout_->setColumnStretch(0, 1);
    // gridLayout_->setColumnStretch(1, 1);
    
    // 将网格布局添加到主布局
    mainLayout_->addLayout(gridLayout_);
    
    // 设置布局
    setLayout(mainLayout_);
    
    // 连接信号槽
    connect(stateCartesianEdit_, &UiStateCartesian::stateCartesianChanged, this, &UiMotionTwoBody::apply);
    connect(stepSizeEdit_, &UiQuantity::quantityChanged, this, &UiMotionTwoBody::apply);
    connect(timeIntervalEdit_, &UiTimeInterval::timeIntervalChanged, this, &UiMotionTwoBody::apply);
    connect(propagationFrameCombo_, &QComboBox::currentTextChanged, this, &UiMotionTwoBody::apply);
    connect(coordTypeCombo_, &QComboBox::currentTextChanged, this, &UiMotionTwoBody::apply);
}

MotionTwoBody* UiMotionTwoBody::getMotionTwoBody() const
{
    return dynamic_cast<MotionTwoBody*>(getObject());
}

void UiMotionTwoBody::setMotionTwoBody(MotionTwoBody* motion)
{
    setObject(motion);
    refreshUi();
}

void UiMotionTwoBody::refreshUi()
{
    if (auto motionTwoBody = getMotionTwoBody())
    {
        // 这里可以添加代码，根据MotionTwoBody的状态更新UI
        // 例如，更新时间、位置、速度等参数
        A_UNUSED(motionTwoBody);
    }
}

void UiMotionTwoBody::apply()
{
    if (auto motionTwoBody = getMotionTwoBody())
    {
        applyTo(motionTwoBody);
    }
}

void UiMotionTwoBody::applyTo(MotionTwoBody* motion)
{
    // 这里可以添加代码，将UI上的设置应用到MotionTwoBody对象
    // 例如，设置时间区间、初始状态等
}

AST_NAMESPACE_END