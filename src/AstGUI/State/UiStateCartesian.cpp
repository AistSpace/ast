///
/// @file      UiStateCartesian.cpp
/// @brief     笛卡尔状态编辑界面
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

#include "UiStateCartesian.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstUtil/Unit.hpp"
#include "AstUtil/Quantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>


AST_NAMESPACE_BEGIN

using namespace units;

UiStateCartesian::UiStateCartesian(QWidget *parent) : UiState(parent)
{
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 创建左侧布局（轨道历元、坐标系）
    QVBoxLayout* leftLayout = new QVBoxLayout();
    
    // 轨道历元
    QHBoxLayout* epochLayout = new QHBoxLayout();
    QLabel* epochLabel = new QLabel("轨道历元", this);
    epochEdit_ = new QLineEdit("2026-03-28 00:00:00.000", this);
    epochUnitCombo_ = new QComboBox(this);
    epochUnitCombo_->addItem("UTCG_MM");
    epochLayout->addWidget(epochLabel);
    epochLayout->addWidget(epochEdit_);
    epochLayout->addWidget(epochUnitCombo_);
    leftLayout->addLayout(epochLayout);
    
    // 坐标系
    QHBoxLayout* frameLayout = new QHBoxLayout();
    QLabel* frameLabel = new QLabel("坐标系", this);
    frameCombo_ = new QComboBox(this);
    frameCombo_->addItem("ICRF");
    frameLayout->addWidget(frameLabel);
    frameLayout->addWidget(frameCombo_);
    leftLayout->addLayout(frameLayout);
    
    // 创建右侧布局（位置和速度）
    QVBoxLayout* rightLayout = new QVBoxLayout();
    
    // 位置X
    QHBoxLayout* posXLayout = new QHBoxLayout();
    QLabel* posXLabel = new QLabel("位置X", this);
    posXEdit_ = new UiQuantity(this);
    posXEdit_->setQuantity(Quantity(6678137, m));
    posXLayout->addWidget(posXLabel);
    posXLayout->addWidget(posXEdit_);
    rightLayout->addLayout(posXLayout);
    
    // 位置Y
    QHBoxLayout* posYLayout = new QHBoxLayout();
    QLabel* posYLabel = new QLabel("位置Y", this);
    posYEdit_ = new UiQuantity(this);
    posYEdit_->setQuantity(Quantity(0, m));
    posYLayout->addWidget(posYLabel);
    posYLayout->addWidget(posYEdit_);
    rightLayout->addLayout(posYLayout);
    
    // 位置Z
    QHBoxLayout* posZLayout = new QHBoxLayout();
    QLabel* posZLabel = new QLabel("位置Z", this);
    posZEdit_ = new UiQuantity(this);
    posZEdit_->setQuantity(Quantity(0, m));
    posZLayout->addWidget(posZLabel);
    posZLayout->addWidget(posZEdit_);
    rightLayout->addLayout(posZLayout);
    
    // 速度X
    QHBoxLayout* velXLayout = new QHBoxLayout();
    QLabel* velXLabel = new QLabel("速度X", this);
    velXEdit_ = new UiQuantity(this);
    velXEdit_->setQuantity(Quantity(0, m / s));
    velXLayout->addWidget(velXLabel);
    velXLayout->addWidget(velXEdit_);
    rightLayout->addLayout(velXLayout);
    
    // 速度Y
    QHBoxLayout* velYLayout = new QHBoxLayout();
    QLabel* velYLabel = new QLabel("速度Y", this);
    velYEdit_ = new UiQuantity(this);
    velYEdit_->setQuantity(Quantity(6789.530297717651592, m / s));
    velYLayout->addWidget(velYLabel);
    velYLayout->addWidget(velYEdit_);
    rightLayout->addLayout(velYLayout);
    
    // 速度Z
    QHBoxLayout* velZLayout = new QHBoxLayout();
    QLabel* velZLabel = new QLabel("速度Z", this);
    velZEdit_ = new UiQuantity(this);
    velZEdit_->setQuantity(Quantity(3686.414173013651634, m / s));
    velZLayout->addWidget(velZLabel);
    velZLayout->addWidget(velZEdit_);
    rightLayout->addLayout(velZLayout);
    
    // 创建网格布局，将左侧和右侧布局放在一起
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->addLayout(leftLayout, 0, 0);
    gridLayout->addLayout(rightLayout, 0, 1);
    
    mainLayout->addLayout(gridLayout);
    setLayout(mainLayout);
}

void UiStateCartesian::setStateCartesian(StateCartesian* state)
{
    if (state)
    {
        stateCartesian_ = state;
        
        // 设置位置和速度值
        posXEdit_->setQuantity(Quantity(state->x(), m));
        posYEdit_->setQuantity(Quantity(state->y(), m));
        posZEdit_->setQuantity(Quantity(state->z(), m));
        velXEdit_->setQuantity(Quantity(state->vx(), m / s));
        velYEdit_->setQuantity(Quantity(state->vy(), m / s));
        velZEdit_->setQuantity(Quantity(state->vz(), m / s));
        
        // 这里可以添加代码，设置轨道历元、坐标系等其他属性
    }
}

StateCartesian* UiStateCartesian::getStateCartesian() const
{
    if (stateCartesian_)
    {
        // 从界面上获取位置和速度值
        stateCartesian_->setX(posXEdit_->getMagnitude());
        stateCartesian_->setY(posYEdit_->getMagnitude());
        stateCartesian_->setZ(posZEdit_->getMagnitude());
        stateCartesian_->setVx(velXEdit_->getMagnitude());
        stateCartesian_->setVy(velYEdit_->getMagnitude());
        stateCartesian_->setVz(velZEdit_->getMagnitude());
        
        // 这里可以添加代码，获取轨道历元、坐标系等其他属性
    }
    
    return stateCartesian_;
}

AST_NAMESPACE_END