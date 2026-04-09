"""Orbit module for AstPy - Orbital mechanics"""
import math
from typing import Optional
from ..Math.vector import Vector3d, Matrix3d

# Physical constants
MU_EARTH = 398600.4418  # km^3/s^2 (Earth gravitational parameter)

class OrbitalElements:
    """Classical orbital elements"""
    def __init__(self):
        self.semi_major_axis: float = 0.0  # km
        self.eccentricity: float = 0.0
        self.inclination: float = 0.0  # degrees
        self.right_ascension: float = 0.0  # degrees (Ω)
        self.argument_of_periapsis: float = 0.0  # degrees (ω)
        self.true_anomaly: float = 0.0  # degrees (ν)
    
    def __str__(self) -> str:
        return (f"OrbitalElements(a={self.semi_major_axis:.2f}km, e={self.eccentricity:.6f}, "
                f"i={self.inclination:.2f}°, Ω={self.right_ascension:.2f}°, "
                f"ω={self.argument_of_periapsis:.2f}°, ν={self.true_anomaly:.2f}°)")
    
    @property
    def semi_latus_rectum(self) -> float:
        return self.semi_major_axis * (1 - self.eccentricity**2)
    
    @property
    def period(self) -> float:
        if self.semi_major_axis <= 0:
            return 0.0
        return 2 * math.pi * math.sqrt(self.semi_major_axis**3 / MU_EARTH)
    
    @property
    def mean_motion(self) -> float:
        if self.semi_major_axis <= 0:
            return 0.0
        return math.sqrt(MU_EARTH / self.semi_major_axis**3)

class OrbitParameters:
    """Orbit parameter conversions and calculations"""
    
    @staticmethod
    def orbital_elements_to_state(elements: OrbitalElements) -> tuple:
        """Convert orbital elements to position and velocity vectors"""
        a = elements.semi_major_axis
        e = elements.eccentricity
        i = math.radians(elements.inclination)
        Omega = math.radians(elements.right_ascension)
        omega = math.radians(elements.argument_of_periapsis)
        nu = math.radians(elements.true_anomaly)
        
        # Semi-latus rectum
        p = a * (1 - e**2)
        
        # Distance from central body
        r = p / (1 + e * math.cos(nu))
        
        # Position in orbital plane
        x_orb = r * math.cos(nu)
        y_orb = r * math.sin(nu)
        
        # Velocity magnitude
        v = math.sqrt(MU_EARTH * (2/r - 1/a))
        
        # Flight path angle
        gamma = math.atan2(e * math.sin(nu), 1 + e * math.cos(nu))
        
        # Velocity components in orbital plane
        vx_orb = -math.sin(nu - gamma) * v
        vy_orb = (math.cos(nu - gamma)) * v
        
        # Rotation matrices
        cos_O, sin_O = math.cos(Omega), math.sin(Omega)
        cos_i, sin_i = math.cos(i), math.sin(i)
        cos_o, sin_o = math.cos(omega), math.sin(omega)
        
        # Position in inertial frame
        Q11 = cos_o * cos_O - sin_o * sin_O * cos_i
        Q12 = -sin_o * cos_O - cos_o * sin_O * cos_i
        Q21 = cos_o * sin_O + sin_o * cos_O * cos_i
        Q22 = -sin_o * sin_O + cos_o * cos_O * cos_i
        Q31 = sin_o * sin_i
        Q32 = cos_o * sin_i
        
        position = Vector3d(
            Q11 * x_orb + Q12 * y_orb,
            Q21 * x_orb + Q22 * y_orb,
            Q31 * x_orb + Q32 * y_orb
        )
        
        velocity = Vector3d(
            Q11 * vx_orb + Q12 * vy_orb,
            Q21 * vx_orb + Q22 * vy_orb,
            Q31 * vx_orb + Q32 * vy_orb
        )
        
        return position, velocity
    
    @staticmethod
    def state_to_orbital_elements(position: Vector3d, velocity: Vector3d) -> OrbitalElements:
        """Convert position and velocity vectors to orbital elements"""
        r = position.norm()
        v = velocity.norm()
        
        # Specific angular momentum
        h = position.cross(velocity)
        h_mag = h.norm()
        
        # Eccentricity vector
        e_vec = (velocity.cross(h) / MU_EARTH) - (position * (1/r))
        e = e_vec.norm()
        
        # Semi-major axis
        a = 1 / (2/r - v**2 / MU_EARTH)
        
        # Inclination
        i = math.acos(h.z / h_mag)
        
        # Node vector
        n = Vector3d(-h.y, h.x, 0)
        n_mag = n.norm()
        
        # Right ascension of ascending node
        if n_mag > 1e-10:
            Omega = math.acos(n.x / n_mag)
            if n.y < 0:
                Omega = 2 * math.pi - Omega
        else:
            Omega = 0.0
        
        # Argument of periapsis
        if n_mag > 1e-10 and e > 1e-10:
            omega = math.acos(n.dot(e_vec) / (n_mag * e))
            if e_vec.z < 0:
                omega = 2 * math.pi - omega
        else:
            omega = 0.0
        
        # True anomaly
        if e > 1e-10:
            nu = math.acos(e_vec.dot(position) / (e * r))
            if position.dot(velocity) < 0:
                nu = 2 * math.pi - nu
        else:
            nu = math.acos(n.dot(position) / (n_mag * r))
            if position.dot(velocity) < 0:
                nu = 2 * math.pi - nu
        
        elements = OrbitalElements()
        elements.semi_major_axis = a
        elements.eccentricity = e
        elements.inclination = math.degrees(i)
        elements.right_ascension = math.degrees(Omega)
        elements.argument_of_periapsis = math.degrees(omega)
        elements.true_anomaly = math.degrees(nu)
        
        return elements
