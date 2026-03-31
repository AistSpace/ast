///
/// @file      UiStateKeplerian.cpp
/// @brief     开普勒轨道根数状态编辑界面
/// @author    axel
/// @date      2026-03-31
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

#include "UiStateKeplerian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstUtil/Unit.hpp"
#include "AstUtil/Quantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

AST_NAMESPACE_BEGIN

using namespace units;

UiStateKeplerian::UiStateKeplerian(QWidget *parent) : UiState(parent)
{
    // 创建主布局
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 2);
    mainLayout->setColumnStretch(2, 3);
    
    int row = 0;
    
    // 轨道历元
    QLabel* epochLabel = new QLabel(tr("轨道历元"), this);
    epochEdit_ = new UiTimePoint(this);
    mainLayout->addWidget(epochLabel, row, 0);
    mainLayout->addWidget(epochEdit_, row, 2);
    row++;
    
    // 坐标系
    QLabel* frameLabel = new QLabel(tr("坐标系"), this);
    frameCombo_ = new QComboBox(this);
    frameCombo_->addItem(tr("ICRF"));
    mainLayout->addWidget(frameLabel, row, 0);
    mainLayout->addWidget(frameCombo_, row, 2);
    row++;
    
    // 轨道大小
    sizeLabel_ = new QLabel(tr("轨道大小"), this);
    sizeTypeCombo_ = new QComboBox(this);
    sizeTypeCombo_->addItem(tr("半长轴"));
    sizeTypeCombo_->addItem(tr("远地点高度"));
    sizeTypeCombo_->addItem(tr("远地点半径"));
    sizeTypeCombo_->addItem(tr("近地点高度"));
    sizeTypeCombo_->addItem(tr("近地点半径"));
    sizeTypeCombo_->addItem(tr("周期"));
    sizeTypeCombo_->addItem(tr("平均角速度"));
    sizeEdit_ = new UiQuantity(this);
    sizeEdit_->setQuantity(Quantity(6678137));
    mainLayout->addWidget(sizeLabel_, row, 0);
    mainLayout->addWidget(sizeTypeCombo_, row, 1);
    mainLayout->addWidget(sizeEdit_, row, 2);
    row++;
    
    // 轨道形状
    shapeLabel_ = new QLabel(tr("轨道形状"), this);
    shapeTypeCombo_ = new QComboBox(this);
    shapeTypeCombo_->addItem(tr("偏心率"));
    shapeTypeCombo_->addItem(tr("远地点高度"));
    shapeTypeCombo_->addItem(tr("远地点半径"));
    shapeTypeCombo_->addItem(tr("近地点高度"));
    shapeTypeCombo_->addItem(tr("近地点半径"));
    shapeEdit_ = new UiQuantity(this);
    shapeEdit_->setQuantity(Quantity(0.0));
    mainLayout->addWidget(shapeLabel_, row, 0);
    mainLayout->addWidget(shapeTypeCombo_, row, 1);
    mainLayout->addWidget(shapeEdit_, row, 2);
    row++;
    
    // 倾角
    incLabel_ = new QLabel(tr("倾角"), this);
    incEdit_ = new UiQuantity(this);
    incEdit_->setQuantity(Quantity(0.0, deg));
    mainLayout->addWidget(incLabel_, row, 0);
    mainLayout->addWidget(incEdit_, row, 2);
    row++;
    
    // 轨道面方向
    orientationLabel_ = new QLabel(tr("轨道面方向"), this);
    orientationTypeCombo_ = new QComboBox(this);
    orientationTypeCombo_->addItem(tr("升交点赤经"));
    orientationTypeCombo_->addItem(tr("升交点经度"));
    orientationEdit_ = new UiQuantity(this);
    orientationEdit_->setQuantity(Quantity(0.0, deg));
    mainLayout->addWidget(orientationLabel_, row, 0);
    mainLayout->addWidget(orientationTypeCombo_, row, 1);
    mainLayout->addWidget(orientationEdit_, row, 2);
    row++;
    
    // 近地点幅角
    argPeriLabel_ = new QLabel(tr("近地点幅角"), this);
    argPeriEdit_ = new UiQuantity(this);
    argPeriEdit_->setQuantity(Quantity(0.0, deg));
    mainLayout->addWidget(argPeriLabel_, row, 0);
    mainLayout->addWidget(argPeriEdit_, row, 2);
    row++;
    
    // 轨道位置
    positionLabel_ = new QLabel(tr("轨道位置"), this);
    positionTypeCombo_ = new QComboBox(this);
    positionTypeCombo_->addItem(tr("真近点角"));
    positionTypeCombo_->addItem(tr("平近点角"));
    positionTypeCombo_->addItem(tr("偏近点角"));
    positionTypeCombo_->addItem(tr("纬度幅角"));
    positionTypeCombo_->addItem(tr("过近地点后时间"));
    positionTypeCombo_->addItem(tr("过升交点后时间"));
    positionTypeCombo_->addItem(tr("过近地点时刻"));
    positionTypeCombo_->addItem(tr("过升交点时刻"));
    positionEdit_ = new UiQuantity(this);
    positionEdit_->setQuantity(Quantity(0.0, deg));
    mainLayout->addWidget(positionLabel_, row, 0);
    mainLayout->addWidget(positionTypeCombo_, row, 1);
    mainLayout->addWidget(positionEdit_, row, 2);
    row++;
    
    setLayout(mainLayout);

    // 连接信号槽
    connect(epochEdit_, &UiTimePoint::timePointChanged, this, &UiStateKeplerian::apply);
    connect(sizeEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::apply);
    connect(sizeTypeCombo_, &QComboBox::currentTextChanged, this, &UiStateKeplerian::apply);
    connect(shapeEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::apply);
    connect(shapeTypeCombo_, &QComboBox::currentTextChanged, this, &UiStateKeplerian::apply);
    connect(incEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::apply);
    connect(orientationEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::apply);
    connect(orientationTypeCombo_, &QComboBox::currentTextChanged, this, &UiStateKeplerian::apply);
    connect(argPeriEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::apply);
    connect(positionEdit_, &UiQuantity::quantityChanged, this, &UiStateKeplerian::apply);
    connect(positionTypeCombo_, &QComboBox::currentTextChanged, this, &UiStateKeplerian::apply);
}

void UiStateKeplerian::refreshUi()
{
    if(auto state = getStateKeplerian()){
        // 设置轨道历元
        TimePoint timePoint = state->getStateEpoch();
        epochEdit_->setTimePoint(timePoint);
        
        // 设置轨道大小
        double sizeParam = state->getSizeParam();
        ESizeType sizeType = static_cast<ESizeType>(sizeTypeCombo_->currentIndex());
        state->setSizeType(sizeType);
        switch(sizeType){
        case ESizeType::eSMA:
            sizeEdit_->setQuantity(Quantity(sizeParam, m));
            break;
        case ESizeType::eApoAlt:
        case ESizeType::ePeriAlt:
            sizeEdit_->setQuantity(Quantity(sizeParam, m));
            break;
        case ESizeType::eApoRad:
        case ESizeType::ePeriRad:
            sizeEdit_->setQuantity(Quantity(sizeParam, m));
            break;
        case ESizeType::ePeriod:
            sizeEdit_->setQuantity(Quantity(sizeParam, s));
            break;
        case ESizeType::eMeanMotion:
            sizeEdit_->setQuantity(Quantity(sizeParam, rad / s));
            break;
        }
        
        // 设置轨道形状
        double shapeParam = state->getShapeParam();
        EShapeType shapeType = static_cast<EShapeType>(shapeTypeCombo_->currentIndex());
        state->setShapeType(shapeType);
        switch(shapeType){
        case EShapeType::eEcc:
            shapeEdit_->setQuantity(Quantity(shapeParam));
            break;
        case EShapeType::eApoAlt:
        case EShapeType::ePeriAlt:
            shapeEdit_->setQuantity(Quantity(shapeParam, m));
            break;
        case EShapeType::eApoRad:
        case EShapeType::ePeriRad:
            shapeEdit_->setQuantity(Quantity(shapeParam, m));
            break;
        }
        
        // 设置倾角
        incEdit_->setQuantity(Quantity(state->getInc(), deg));
        
        // 设置轨道面方向
        double orientationParam = state->getOrientationParam();
        EOrientationType orientationType = static_cast<EOrientationType>(orientationTypeCombo_->currentIndex());
        state->setOrientationType(orientationType);
        orientationEdit_->setQuantity(Quantity(orientationParam, deg));
        
        // 设置近地点幅角
        argPeriEdit_->setQuantity(Quantity(state->getArgPeri(), deg));
        
        // 设置轨道位置
        double positionParam = state->getPositionParam();
        EPositionType positionType = static_cast<EPositionType>(positionTypeCombo_->currentIndex());
        state->setPositionType(positionType);
        switch(positionType){
        case EPositionType::eTrueAnomaly:
        case EPositionType::eMeanAnomaly:
        case EPositionType::eEccAnomaly:
        case EPositionType::eArgLat:
            positionEdit_->setQuantity(Quantity(positionParam, deg));
            break;
        case EPositionType::eTimePastPeri:
        case EPositionType::eTimePastAscNode:
            positionEdit_->setQuantity(Quantity(positionParam, s));
            break;
        case EPositionType::eTimeOfPeriPassage:
        case EPositionType::eTimeOfAscNodePassage:
            // 这里需要特殊处理，因为是时间点
            break;
        }
    }
}

void UiStateKeplerian::apply()
{
    if (auto state = getStateKeplerian())
    {
        applyTo(state);
        emit stateKeplerianChanged(state);
    }
}

void UiStateKeplerian::applyTo(StateKeplerian *state)
{
    // 获取轨道历元
    TimePoint timePoint = epochEdit_->getTimePoint();
    state->setStateEpoch(timePoint);
    
    // 设置轨道大小
    ESizeType sizeType = static_cast<ESizeType>(sizeTypeCombo_->currentIndex());
    state->setSizeType(sizeType);
    double sizeParam = sizeEdit_->getValueSI();
    state->setSizeParam(sizeParam, sizeType);
    
    // 设置轨道形状
    EShapeType shapeType = static_cast<EShapeType>(shapeTypeCombo_->currentIndex());
    state->setShapeType(shapeType);
    double shapeParam = shapeEdit_->getValueSI();
    state->setShapeParam(shapeParam, shapeType);
    
    // 设置倾角
    state->setInc(incEdit_->getValueSI());
    
    // 设置轨道面方向
    EOrientationType orientationType = static_cast<EOrientationType>(orientationTypeCombo_->currentIndex());
    state->setOrientationType(orientationType);
    double orientationParam = orientationEdit_->getValueSI();
    state->setOrientationParam(orientationParam, orientationType);
    
    // 设置近地点幅角
    state->setArgPeri(argPeriEdit_->getValueSI());
    
    // 设置轨道位置
    EPositionType positionType = static_cast<EPositionType>(positionTypeCombo_->currentIndex());
    state->setPositionType(positionType);
    double positionParam = positionEdit_->getValueSI();
    state->setPositionParam(positionParam, positionType);
}

void UiStateKeplerian::setStateKeplerian(StateKeplerian* state)
{
    if (state)
    {
        setObject(state);
        refreshUi();
    }
}

StateKeplerian* UiStateKeplerian::getStateKeplerian() const
{
    return dynamic_cast<StateKeplerian*>(getObject());
}

AST_NAMESPACE_END