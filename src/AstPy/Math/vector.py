"""Math module for AstPy - Vector and Matrix operations"""
import math
from typing import Tuple

class Vector3d:
    """3D Vector class"""
    def __init__(self, x: float = 0.0, y: float = 0.0, z: float = 0.0):
        self.x = x
        self.y = y
        self.z = z
    
    def __str__(self) -> str:
        return f"Vector3d({self.x}, {self.y}, {self.z})"
    
    def __add__(self, other: 'Vector3d') -> 'Vector3d':
        return Vector3d(self.x + other.x, self.y + other.y, self.z + other.z)
    
    def __sub__(self, other: 'Vector3d') -> 'Vector3d':
        return Vector3d(self.x - other.x, self.y - other.y, self.z - other.z)
    
    def __mul__(self, scalar: float) -> 'Vector3d':
        return Vector3d(self.x * scalar, self.y * scalar, self.z * scalar)
    
    def norm(self) -> float:
        return math.sqrt(self.x**2 + self.y**2 + self.z**2)
    
    def dot(self, other: 'Vector3d') -> float:
        return self.x * other.x + self.y * other.y + self.z * other.z
    
    def cross(self, other: 'Vector3d') -> 'Vector3d':
        return Vector3d(
            self.y * other.z - self.z * other.y,
            self.z * other.x - self.x * other.z,
            self.x * other.y - self.y * other.x
        )
    
    def normalize(self) -> 'Vector3d':
        n = self.norm()
        if n > 0:
            return Vector3d(self.x / n, self.y / n, self.z / n)
        return Vector3d()

class Matrix3d:
    """3x3 Matrix class"""
    def __init__(self, data: Tuple = None):
        if data:
            self.data = list(data)
        else:
            self.data = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
    
    def __str__(self) -> str:
        return f"Matrix3d({self.data[:3]}, {self.data[3:6]}, {self.data[6:9]})"
    
    def __mul__(self, other: 'Matrix3d') -> 'Matrix3d':
        result = []
        for i in range(3):
            for j in range(3):
                val = sum(self.data[i*3+k] * other.data[k*3+j] for k in range(3))
                result.append(val)
        return Matrix3d(tuple(result))
    
    def __mul__(self, vector: Vector3d) -> Vector3d:
        x = self.data[0] * vector.x + self.data[1] * vector.y + self.data[2] * vector.z
        y = self.data[3] * vector.x + self.data[4] * vector.y + self.data[5] * vector.z
        z = self.data[6] * vector.x + self.data[7] * vector.y + self.data[8] * vector.z
        return Vector3d(x, y, z)
    
    def determinant(self) -> float:
        return (self.data[0] * (self.data[4]*self.data[8] - self.data[5]*self.data[7])
                - self.data[1] * (self.data[3]*self.data[8] - self.data[5]*self.data[6])
                + self.data[2] * (self.data[3]*self.data[7] - self.data[4]*self.data[6]))
    
    @staticmethod
    def identity() -> 'Matrix3d':
        return Matrix3d()
    
    @staticmethod
    def rotation_x(angle: float) -> 'Matrix3d':
        c, s = math.cos(angle), math.sin(angle)
        return Matrix3d((1, 0, 0, 0, c, -s, 0, s, c))
    
    @staticmethod
    def rotation_y(angle: float) -> 'Matrix3d':
        c, s = math.cos(angle), math.sin(angle)
        return Matrix3d((c, 0, s, 0, 1, 0, -s, 0, c))
    
    @staticmethod
    def rotation_z(angle: float) -> 'Matrix3d':
        c, s = math.cos(angle), math.sin(angle)
        return Matrix3d((c, -s, 0, s, c, 0, 0, 0, 1))
