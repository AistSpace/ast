///
/// @file      testUiCelestialBody.cpp
/// @brief     测试 UiCelestialBody 类
/// @details   测试天体编辑界面功能
/// @author    Aist
/// @date      2026-04-10
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#include "AstGUI/UiCelestialBody.hpp"
#include "AstCore/CelestialBody.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    printf("testUiCelestialBody.cpp\n");
    QApplication app(argc, argv);
    
    QMainWindow window;
    QWidget* centralWidget = new QWidget(&window);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    // 创建 UiCelestialBody 组件
    UiCelestialBody* uiCelestialBody = new UiCelestialBody(centralWidget);
    layout->addWidget(uiCelestialBody);
    
    // 创建按钮区
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* testButton = new QPushButton("测试", centralWidget);
    QPushButton* refreshButton = new QPushButton("刷新", centralWidget);
    buttonLayout->addWidget(testButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);
    
    // 创建土卫四 (Dione) 天体
    CelestialBody* dione = new CelestialBody("Dione");
    dione->setJplSpiceId(604);  // 土卫四的NAIF ID
    
    // 绑定天体
    uiCelestialBody->setCelestialBody(dione);
    
    // 测试按钮
    QObject::connect(testButton, &QPushButton::clicked, [uiCelestialBody, dione]() {
        CelestialBody* body = uiCelestialBody->getCelestialBody();
        if (body)
        {
            qDebug() << "=== CelestialBody Info ===";
            qDebug() << "Name:" << QString::fromStdString(body->getName());
            qDebug() << "SPICE ID:" << body->getJplSpiceId();
            qDebug() << "GM:" << body->getGM() << "m³/s²";
            qDebug() << "Radius:" << body->getRadius() << "m";
            qDebug() << "Gravity Model:" << QString::fromStdString(body->getGravityModel());
        }
    });
    
    // 刷新按钮
    QObject::connect(refreshButton, &QPushButton::clicked, [uiCelestialBody]() {
        uiCelestialBody->refreshUi();
    });
    
    // 天体变更信号
    QObject::connect(uiCelestialBody, &UiCelestialBody::celestialBodyChanged, [](CelestialBody* body) {
        qDebug() << "CelestialBody changed:" << QString::fromStdString(body->getName());
    });
    
    window.setCentralWidget(centralWidget);
    window.setWindowTitle("UiCelestialBody Test - Dione");
    window.resize(500, 700);
    window.show();
    
    return app.exec();
}
