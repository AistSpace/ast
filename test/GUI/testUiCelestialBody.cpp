///
/// @file      testUiCelestialBody.cpp
/// @brief     测试 UiCelestialBody 类
/// @details   测试天体编辑界面的功能
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
#include <QLabel>

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    printf("testUiCelestialBody.cpp\n");
    QApplication app(argc, argv);
    
    QMainWindow window;
    QWidget* centralWidget = new QWidget(&window);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    // 创建标题
    QLabel* titleLabel = new QLabel("天体编辑测试", centralWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // 创建 UiCelestialBody 组件
    UiCelestialBody* uiCelestialBody = new UiCelestialBody(centralWidget);
    layout->addWidget(uiCelestialBody);
    
    // 创建测试按钮
    QPushButton* testButton = new QPushButton("测试", centralWidget);
    layout->addWidget(testButton);
    
    // 创建 CelestialBody 对象（地球）
    CelestialBody* earth = new CelestialBody("Earth");
    earth->setJplSpiceId(399);  // 地球的NAIF ID
    
    // 绑定 CelestialBody 对象
    uiCelestialBody->setCelestialBody(earth);
    
    QObject::connect(testButton, &QPushButton::clicked, [uiCelestialBody, earth]() {
        CelestialBody* body = uiCelestialBody->getCelestialBody();
        if (body)
        {
            qDebug() << "CelestialBody Info:";
            qDebug() << "Name:" << QString::fromStdString(body->getName());
            qDebug() << "JPL SPICE ID:" << body->getJplSpiceId();
            qDebug() << "Radius:" << body->getRadius();
            qDebug() << "GM:" << body->getGM();
        }
    });
    
    window.setCentralWidget(centralWidget);
    window.setWindowTitle("UiCelestialBody Test");
    window.resize(600, 400);
    window.show();
    
    return app.exec();
}
