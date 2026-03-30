# Attitude

*Note: This document is AI-translated. Please refer to the source code for specific APIs and behaviors.*

## Overview

The Attitude module provides complete attitude representation and conversion functionality, supporting three main attitude representation methods: quaternions, Euler angles, and rotation matrices, with conversions between them.

## Core Concepts

### Attitude Representation Methods

#### 1. Quaternion
- Singularity-free attitude representation
- Suitable for interpolation and continuous rotation
- Low memory footprint (4 doubles)

#### 2. Euler Angles
- Intuitive attitude representation (roll, pitch, yaw)
- Supports 12 rotation sequences
- May have gimbal lock issues

#### 3. Rotation Matrix (Matrix3d)
- 3×3 orthogonal matrix
- Suitable for vector transformation
- No singularity issues

### Rotation Sequences

The module supports 12 Euler angle rotation sequences:

| Type | Rotation Sequence | Enum Value |
|------|------------------|------------|
| Tait-Bryan angles | X→Y→Z | `eXYZ = 123` |
| Tait-Bryan angles | X→Z→Y | `eXZY = 132` |
| Tait-Bryan angles | Y→X→Z | `eYXZ = 213` |
| Tait-Bryan angles | Y→Z→X | `eYZX = 231` |
| Tait-Bryan angles | Z→X→Y | `eZXY = 312` |
| Tait-Bryan angles | Z→Y→X | `eZYX = 321` |
| Classic Euler angles | X→Y→X | `eXYX = 121` |
| Classic Euler angles | X→Z→X | `eXZX = 131` |
| Classic Euler angles | Y→X→Y | `eYXY = 212` |
| Classic Euler angles | Y→Z→Y | `eYZY = 232` |
| Classic Euler angles | Z→X→Z | `eZXZ = 313` |
| Classic Euler angles | Z→Y→Z | `eZYZ = 323` |

**Note**: Tait-Bryan angles use three different axes, while classic Euler angles use the same first and third axes.

## Usage Examples

### Example 1: Basic Conversion

@[code](/examples/math/attitude/basic_convert.cpp)

### Example 2: Euler Angle Usage

@[code](/examples/math/attitude/euler.cpp)

### Example 3: Quaternion Operations

@[code](/examples/math/attitude/quaternion.cpp)

### Example 4: Complete Conversion Flow

@[code](/examples/math/attitude/example_convert.cpp)

## Angle Range Description

The valid ranges of Euler angles differ depending on the rotation sequence type:

- **Tait-Bryan angles** (ABC sequence):
  - `angle2` in the interval `[-π/2, π/2]`
  - `angle1`, `angle3` in the interval `[-π, π]`

- **Classic Euler angles** (ABA sequence):
  - `angle2` in the interval `[0, π]` 
  - `angle1`, `angle3` in the interval `[-π, π]`

## Error Handling

Conversion functions return `errc_t` type to indicate operation status, and callers should check the return value to ensure successful conversion.

## Dependencies

- `AstGlobal.h`: Project global definitions
- `MathOperator.hpp`: Mathematical operation functions
- `Matrix3d`, `Vector3d`: Matrix and vector types

## API Reference

@[code](/src/AstMath/Attitude/AttitudeConvert.hpp)