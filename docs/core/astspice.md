# AstSPICE模块

*说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。*

## 模块概述

AstSPICE模块提供了与NASA SPICE（Spacecraft Planet Instrument C-matrix Events）工具包接口兼容的函数封装。SPICE是航天任务中广泛使用的工具包，用于计算星历、坐标系变换、行星状态等航天动力学相关参数。AstSPICE模块将这些功能以现代C++风格封装，便于在C++项目中集成使用。

## 核心概念

### SPICE简介

SPICE是由NASA导航和天文信息服务部门（NAIF）开发的工具包，主要包含以下几类数据文件（Kernel）：
- **SPK (Spacecraft Planet Kernel)**：星历数据，记录天体位置和速度
- **PCK (Planetary Constants Kernel)**：行星常数，如引力参数、自转参数等
- **FK (Frame Kernel)**：坐标系定义
- **CK (C-matrix Kernel)**：姿态数据
- **IK (Instrument Kernel)**：仪器参数
- **LSK (Leapseconds Kernel)**：闰秒数据

### 常用参考系

AstSPICE支持多种标准参考系：
- **J2000**：儒略日2000年1月1日12:00 TT的惯性参考系
- **ICRF**：国际天球参考系，是J2000的现代实现
- **ECLIPJ2000**：黄道J2000惯性系
- **EM**：地月系质心惯性系
- **IAU_EARTH**：国际天文学联合会定义的地球固连系

### 时间系统

- **ET (Ephemeris Time)**：星历时间，动力学时
- **UTC (Coordinated Universal Time)**：协调世界时
- **TT (Terrestrial Time)**：地球时

## 主要功能

### 1. 坐标变换

#### 1.1 旋转矩阵变换 - pxform

计算两个参考系之间的旋转矩阵：

```cpp
errc_t pxform(
    StringView   from,      // 源参考系名称
    StringView   to,        // 目标参考系名称
    const TimePoint& et,    // 时间点
    Matrix3d&    rotate     // 输出旋转矩阵
);
```

#### 1.2 旋转轴与角度 - axisar

根据旋转轴和角度计算旋转矩阵：

```cpp
void axisar(
    const Vector3d& axis,   // 旋转轴
    double         angle,   // 旋转角度（弧度）
    Matrix3d&      r        // 输出旋转矩阵
);
```

### 2. 坐标系转换

#### 2.1 球坐标/直角坐标转换

| 函数 | 功能 |
|------|------|
| latrec | 球坐标（半径、经度、纬度）转直角坐标 |
| radrec | 距离-赤经-赤纬转直角坐标 |
| azlrec | 距离-方位角-俯仰角转直角坐标 |

#### 2.2 矩阵/欧拉角/四元数转换

| 函数 | 功能 |
|------|------|
| eul2m | 欧拉角转旋转矩阵 |
| m2eul | 旋转矩阵转欧拉角 |
| m2q | 旋转矩阵转四元数 |
| q2m | 四元数转旋转矩阵 |

### 3. 星历计算

#### 3.1 天体位置计算 - spkpos

计算目标天体相对于观测者的位置：

```cpp
errc_t spkpos(
    CelestialBody*  targ,    // 目标天体
    const TimePoint& et,     // 时间点
    Axes*           ref,     // 参考轴系
    StringView       abcorr,  // 像差校正选项
    CelestialBody*  obs,     // 观测者
    Vector3d&        ptarg,   // 输出位置
    double*         lt = nullptr  // 光行时（可选）
);
```

像差校正选项（abcorr）：
- "NONE"：无校正
- "LT"：光行时校正
- "LT+S"：光行时+恒星像差校正
- "CN"：天文台网校正
- "CN+S"：天文台网+恒星像差校正
- "XLT": 光行时校正（高精度，X表示高精度模式）
- "XLT+S": 光行时+恒星像差校正（高精度）
- "XCN": 天文台网校正（高精度）
- "XCN+S": 天文台网+恒星像差校正（高精度）

#### 3.2 天体位置速度计算 - spkgeo

计算天体位置和速度：

```cpp
void spkgeo(
    CelestialBody*  targ,    // 目标天体
    const TimePoint& et,     // 时间点
    Axes*           ref,     // 参考轴系
    CelestialBody*  obs,     // 观测者
    CartState&      state,  // 输出位置速度状态
    double*         lt = nullptr  // 光行时（可选）
);
```

#### 3.3 太阳系质心状态 - spkssb

```cpp
errc_t spkssb(
    CelestialBody*  targ,    // 目标天体
    const TimePoint& et,     // 时间点
    Axes*           ref,     // 参考轴系
    CartState&      starg   // 输出状态
);
```

### 4. 姿态计算

#### 4.1 恒星位置计算 - stelab

计算恒星表观位置（考虑光行差和视差）：

```cpp
errc_t stelab(
    const Vector3d& pobj,   // 恒星位置向量
    const Vector3d& vobs,   // 观测者速度
    Vector3d&       appobj  // 输出表观位置
);
```

### 5. 矩阵运算

#### 5.1 矩阵乘法 - mxm

```cpp
void mxm(
    const Matrix3d& m1,      // 矩阵1
    const Matrix3d& m2,      // 矩阵2
    Matrix3d&       mout    // 输出矩阵
);
```

#### 5.2 向量旋转 - vrotv

绕指定轴旋转向量：

```cpp
void vrotv(
    const Vector3d& v,       // 输入向量
    const Vector3d& axis,    // 旋转轴
    double         theta,   // 旋转角度（弧度）
    Vector3d&      r         // 输出向量
);
```

### 6. 时间转换

#### 6.1 UTC转ET - utc2et

```cpp
errc_t utc2et(
    StringView  utcstr,      // UTC时间字符串
    double&     et           // 输出星历时间
);
```

### 7. 旋转矩阵转状态变换 - rav2xf

```cpp
void rav2xf(
    const Matrix3d& rot,     // 旋转矩阵
    const Vector3d&  av,      // 角速度向量
    Matrix6d&       xform    // 输出6x6状态变换矩阵
);
```

### 8. 运行时查询

#### 8.1 天体查询

```cpp
CelestialBody* aSpiceFindBody(StringView name);  // 按名称查找
CelestialBody* aSpiceFindBody(int id);             // 按ID查找
```

#### 8.2 参考系查询

```cpp
Axes* aSpiceFindAxes(StringView name);  // 按名称查找参考系
```

## 常量与历元

| 函数 | 值/说明 | 说明 |
|------|--------|------|
| `clight()` | `~299792458.0 m/s` | 光速 (真空) |
| j2000 | 2451545.0 | J2000儒略日 |
| j1900 | 2415020.31352 | J1900儒略日 |
| j1950 | 2433282.4235 | J1950儒略日 |
| b1900 | 2415020.31352 | B1900贝塞尔日 |
| b1950 | 2433282.4235 | B1950贝塞尔日 |
| dpr | ~57.295779513 | 弧度转角度因子 |
| jyear | 31557600.0 | 儒略年秒数 |

## 文件结构

```
src/AstSPICE/
  README.md                    # 模块说明
  SpiceUsr.h                   # C接口声明
  SpiceUsr.hpp                 # C++接口声明
  SpiceZpr.h                   # C SPICE函数实现声明
  SpiceZpr.hpp                 # C++ SPICE函数实现
  SpiceZpr.cpp                 # SPICE函数实现
  xmake.lua                     # 构建配置
  RunTime/
    SpiceRunTime.hpp           # 运行时查询接口
    SpiceRunTime.cpp
    SpiceBodyRegistry.hpp      # 天体注册表
    SpiceBodyRegistry.cpp
    SpiceFrameRegistry.hpp     # 参考系注册表
    SpiceFrameRegistry.cpp
```

## 依赖关系

- AstGlobal.h：项目全局定义和宏
- AstCore/Time.hpp：时间系统定义
- AstCore/Axes.hpp：轴系定义
- AstCore/Matrix.hpp：矩阵运算
- AstCore/Quaternion.hpp：四元数定义
- AstCore/CelestialBody.hpp：天体定义
- AstCore/CartState.hpp：笛卡尔状态定义

## 使用示例

### 示例1：旋转轴与角度 - axisar

使用axisar函数根据旋转轴和角度计算旋转矩阵：

@[code](/examples/core/astspice/axisar_rotation.cpp)

### 示例2：坐标转换 - latrec/radrec

球坐标（半径、经度、纬度）和直角坐标的相互转换：

@[code](/examples/core/astspice/coordinate_convert.cpp)

### 示例3：矩阵/欧拉角/四元数转换

欧拉角、旋转矩阵和四元数之间的相互转换：

@[code](/examples/core/astspice/matrix_euler_quat.cpp)

### 示例4：向量旋转与矩阵乘法

使用vrotv函数绕指定轴旋转向量，以及使用mxm进行矩阵乘法：

@[code](/examples/core/astspice/vector_rotation.cpp)

## 注意事项

1. **Kernel文件**：使用SPICE功能前需要加载相应的Kernel文件（.bsp, .tpc, .tf等）
2. **时间系统**：SPICE内部使用ET（星历时间），与UTC之间需要进行转换
3. **像差校正**：进行高精度计算时需要考虑光行时和恒星像差校正
4. **坐标系名称**：参考系名称需要与加载的Frame Kernel中定义的一致
5. **天体名称**：天体名称需要与SPICE命名的规范一致（如"EARTH"、"SUN"等）
