///
/// @file      testUiQuantity.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-27
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

#include "AstGUI/UiQuantity.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    printf("testUiQuantity.cpp\n");
    QApplication app(argc, argv);
    
    QMainWindow window;
    QWidget* centralWidget = new QWidget(&window);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    // 创建 UiQuantity 组件
    UiQuantity* uiQuantity = new UiQuantity(centralWidget);
    layout->addWidget(uiQuantity);
    
    // 创建测试按钮
    QPushButton* testButton = new QPushButton("测试", centralWidget);
    layout->addWidget(testButton);
    
    QObject::connect(testButton, &QPushButton::clicked, [uiQuantity]() {
        // 测试基本功能
        Quantity q = uiQuantity->quantity();
        qDebug() << "Current quantity:" << q.magnitude() << QString::fromStdString(q.unit().name());
        
        // 测试 getMagnitude 和 setMagnitude
        double magnitude = uiQuantity->getMagnitude();
        qDebug() << "Current magnitude:" << magnitude;
        uiQuantity->setMagnitude(magnitude * 2);
        qDebug() << "After setMagnitude:" << uiQuantity->getMagnitude();
        
        // 测试 getUnit 和 setUnit
        Unit unit = uiQuantity->getUnit();
        qDebug() << "Current unit:" << QString::fromStdString(unit.name());
        
        // 测试 changeUnit
        if (unit == units::m) {
            uiQuantity->changeUnit(units::km);
            qDebug() << "After changeUnit to km:" << uiQuantity->getMagnitude() << QString::fromStdString(uiQuantity->getUnit().name());
        } else {
            uiQuantity->changeUnit(units::m);
            qDebug() << "After changeUnit to m:" << uiQuantity->getMagnitude() << QString::fromStdString(uiQuantity->getUnit().name());
        }
        
        // 测试 getValueSI 和 setValueSI
        double siValue = uiQuantity->getValueSI();
        qDebug() << "SI value:" << siValue;
        uiQuantity->setValueSI(siValue * 2);
        qDebug() << "After setValueSI:" << uiQuantity->getMagnitude() << QString::fromStdString(uiQuantity->getUnit().name());
        
        // 测试 getValueInUnit
        double valueInKm = uiQuantity->getValueInUnit(units::km);
        qDebug() << "Value in km:" << valueInKm;
        
        // 测试 setValueInUnit
        uiQuantity->setValueInUnit(5, units::km);
        qDebug() << "After setValueInUnit (5 km):" << uiQuantity->getMagnitude() << QString::fromStdString(uiQuantity->getUnit().name());
        
        // 测试 setValueUnit
        uiQuantity->setValueUnit(1000, units::m);
        qDebug() << "After setValueUnit (1000 m):" << uiQuantity->getMagnitude() << QString::fromStdString(uiQuantity->getUnit().name());
    });
    
    // 设置默认值
    uiQuantity->setQuantity(Quantity(100, units::m));
    
    window.setCentralWidget(centralWidget);
    window.setWindowTitle("UiQuantity Test");
    window.resize(400, 200);
    window.show();
    
    return app.exec();
}