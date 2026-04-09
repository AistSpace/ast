# Axes轴系模块

*说明：本文档旨在帮助理解Axes轴系模块的功能和使用方法，具体API和行为请查阅源代码。*

## 模块概述

Axes模块提供了统一的坐标系（轴系）抽象和管理功能。在航天动力学中，不同的计算需要在不同的参考系下进行，如惯性系、地球固连系、天体固连系等。Axes模块通过层次化的设计，提供了轴系之间的旋转变换计算能力。

## 核心概念

### 轴系类层次结构

```
Axes (基类)
├── AxesRoot        # 根轴系，层级结构的起点
├── AxesICRF        # 国际天球参考系（J2000的现代实现）
├── AxesJ2000       # J2000惯性系
├── AxesECF         # 地球固连系
├── AxesMOD         # 瞬时平赤道系
├── AxesTOD         # 瞬时真赤道系
├── AxesGTOD        # 格林尼治真赤道系
├── AxesFrozen      # 冻结轴系（在特定时刻冻结的轴系）
├── AxesBodyRelated  # 天体相关轴系的基类
│   ├── AxesBodyInertial  # 天体惯性轴系
│   ├── AxesBodyFixed     # 天体固连轴系
│   ├── AxesBodyMOD       # 天体MOD轴系
│   └── AxesBodyTOD       # 天体TOD轴系
└── ... (其他专用轴系)
```

### 轴系变换原理

每个轴系维护其到父轴系的旋转变换。通过组合从子轴系到父轴系的变换，可以计算出任意两个轴系之间的变换。

### 旋转表示

- **Rotation类**：表示纯旋转变换，包含旋转矩阵
- **KinematicRotation类**：表示运动学旋转，包含旋转矩阵和角速度

## 主要功能

### 1. 获取轴系实例

#### 1.1 全局单例轴系

```cpp
// 获取ICRF惯性系
Axes* icrf = aAxesICRF();

// 获取J2000惯性系
Axes* j2000 = aAxesJ2000();

// 获取ECF地球固连系
Axes* ecf = aAxesECF();

// 获取MOD瞬时平赤道系
Axes* mod = aAxesMOD();

// 获取TOD瞬时真赤道系
Axes* tod = aAxesTOD();

// 获取GTOD格林尼治真赤道系
Axes* gtod = aAxesGTOD();

// 获取根轴系
Axes* root = aAxesRoot();
```

@[code](/examples/core/axes/get_axes_instance.cpp)

#### 1.2 通过名称获取轴系

```cpp
// 通过名称获取内置轴系
Axes* galactic = aGetAxes("GALACTIC");
```

#### 1.3 创建天体相关轴系

```cpp
// 创建天体惯性轴系
auto bodyInertial = AxesBodyInertial::NewShared(earth);

// 创建天体固连轴系
auto bodyFixed = AxesBodyFixed::NewShared(earth);

// 创建天体MOD轴系
auto bodyMOD = AxesBodyMOD::NewShared(earth);

// 创建天体TOD轴系
auto bodyTOD = AxesBodyTOD::NewShared(earth);
```

### 2. 轴系变换计算

#### 2.1 获取旋转变换

```cpp
// 获取轴系到另一个轴系的旋转变换
Rotation rotation;
errc_t err = aAxesTransform(source, target, timePoint, rotation);

// 使用成员函数
source->getTransformTo(target, timePoint, rotation);
```

@[code](/examples/core/axes/get_rotation_transform.cpp)

#### 2.2 获取转换矩阵

```cpp
// 直接获取转换矩阵
Matrix3d matrix;
errc_t err = aAxesTransform(source, target, timePoint, matrix);
```

@[code](/examples/core/axes/get_transform_matrix.cpp)

#### 2.3 获取运动学旋转变换

```cpp
// 获取包含角速度的运动学旋转
KinematicRotation kinRot;
errc_t err = aAxesTransform(source, target, timePoint, kinRot);

// 获取旋转矩阵
Matrix3d rot = kinRot.getRotation();

// 获取角速度向量
Vector3d angVel = kinRot.getRotationRate();
```

@[code](/examples/core/axes/get_kinematic_rotation.cpp)

### 3. 冻结轴系

冻结轴系用于在特定时刻"冻结"某个轴系，之后任何时间获取该轴系的变换都返回冻结时刻的值。

#### 3.1 创建冻结轴系

```cpp
// 创建冻结轴系
TimePoint freezeTime = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
Axes* frozenAxes = AxesFrozen::New(ecf, freezeTime, aAxesICRF());

// 使用共享指针版本
auto frozenAxesShared = AxesFrozen::MakeShared(ecf, freezeTime, aAxesICRF());
```

@[code](/examples/core/axes/create_frozen_axes.cpp)

#### 3.2 使用冻结轴系

```cpp
// 任何时刻获取变换都返回冻结时刻的值
Rotation rotation1, rotation2;
frozenAxes->getTransformTo(aAxesICRF(), freezeTime, rotation1);  // 冻结时刻
frozenAxes->getTransformTo(aAxesICRF(), otherTime, rotation2);   // 其他时刻
// rotation1 和 rotation2 相同
```

@[code](/examples/core/axes/use_frozen_axes.cpp)

### 4. 轴系层级操作

#### 4.1 获取父轴系

```cpp
Axes* parent = axes->getParent();
```

#### 4.2 获取轴系深度

```cpp
int depth = axes->getDepth();
```

#### 4.3 获取祖先轴系

```cpp
Axes* ancestor = axes->getAncestor(depth);
```

@[code](/examples/core/axes/axes_hierarchy.cpp)

## 常用轴系说明

| 轴系名称 | 说明 | 父轴系 |
|---------|------|--------|
| ICRF/J2000 | 国际天球参考系，惯性系 | Root |
| ECF | 地球固连系，地球固定坐标系 | TOD |
| MOD | 瞬时平赤道系，瞬时平赤道和平春分点 | J2000 |
| TOD | 瞬时真赤道系，瞬时真赤道和平春分点 | MOD |
| GTOD | 格林尼治真赤道系，格林尼治子午面和真赤道 | J2000 |

## 文件结构

```
src/AstCore/Coordinate/Axes/
├── Axes.hpp                      # 轴系基类定义
├── AxesRoot.hpp/cpp              # 根轴系
├── AxesICRF.hpp/cpp             # ICRF轴系
├── AxesFrozen.hpp/cpp           # 冻结轴系
├── AxesFrozenAtEventTime.hpp/cpp # 事件时间冻结轴系
├── AxesBodyRelated.hpp/cpp       # 天体相关轴系基类
├── AxesBodyInertial.hpp/cpp     # 天体惯性轴系
├── AxesBodyFixed.hpp/cpp        # 天体固连轴系
├── AxesBodyMOD.hpp/cpp          # 天体MOD轴系
├── AxesBodyTOD.hpp/cpp          # 天体TOD轴系
├── AxesMacro.hpp                # 轴系声明宏
├── BuiltinAxes.hpp              # 内置轴系声明
├── BuiltinAxes.cpp              # 内置轴系实现
└── BuiltinAxesRegistry.hpp/cpp # 内置轴系注册表
```

## 依赖关系

- AstCore/Object.hpp：对象基类
- AstCore/TimePoint.hpp：时间点定义
- AstMath/Rotation.hpp：旋转变换
- AstMath/KinematicRotation.hpp：运动学旋转
- AstMath/Matrix.hpp：矩阵运算

## 注意事项

1. **线程安全**：单例轴系实例是线程安全的，但多个线程同时进行轴系变换计算时需要注意同步
2. **时间系统**：轴系变换依赖于准确的时间输入，确保使用正确的TimePoint
3. **精度考虑**：不同轴系之间的变换可能存在微小差异，高精度应用需注意
4. **内存管理**：使用SharedPtr版本可以简化内存管理，避免内存泄漏
