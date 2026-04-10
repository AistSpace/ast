///
/// @file      UiCelestialBody.hpp
/// @brief     天体编辑界面
/// @author    Aist
/// @date      2026-04-10
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"

class QLineEdit;
class QSpinBox;

AST_NAMESPACE_BEGIN

class CelestialBody;

class AST_GUI_API UiCelestialBody: public UiObject
{
    Q_OBJECT
public:
    UiCelestialBody(Object* object, QWidget* parent = nullptr);
    UiCelestialBody(QWidget* parent = nullptr);

    void refreshUi();
    void apply();

    CelestialBody* getCelestialBody() const;
    void setCelestialBody(CelestialBody* body);

signals:
    void celestialBodyChanged(CelestialBody* body);

private:
    QLineEdit* nameEdit_{nullptr};
    QSpinBox* idSpin_{nullptr};
    QLineEdit* radiusEdit_{nullptr};
    QLineEdit* gmEdit_{nullptr};
};

AST_NAMESPACE_END
