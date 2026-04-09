"""
Time module for AstPy
Provides Date, DateTime, JulianDate, and TimePoint classes
"""
from datetime import datetime
from typing import Union

class Date:
    """Date class for handling calendar dates"""
    def __init__(self, year: int, month: int, day: int):
        self.year = year
        self.month = month
        self.day = day
    
    def __str__(self) -> str:
        return f"{self.year:04d}-{self.month:02d}-{self.day:02d}"
    
    def to_julian_date(self) -> 'JulianDate':
        return self.to_datetime().to_julian_date()
    
    def to_datetime(self) -> 'DateTime':
        return DateTime(self.year, self.month, self.day, 0, 0, 0.0)

class DateTime:
    """DateTime class for handling date and time"""
    def __init__(self, year: int, month: int, day: int, hour: int = 0, minute: int = 0, second: float = 0.0):
        self.year = year
        self.month = month
        self.day = day
        self.hour = hour
        self.minute = minute
        self.second = second
    
    def __str__(self) -> str:
        return f"{self.year:04d}-{self.month:02d}-{self.day:02d} {self.hour:02d}:{self.minute:02d}:{self.second:08.5f}"
    
    def to_julian_date(self) -> 'JulianDate':
        return JulianDate.from_datetime(self)
    
    def to_datetime(self) -> datetime:
        return datetime(self.year, self.month, self.day, self.hour, self.minute, int(self.second))

class JulianDate:
    """Julian Date class for astronomical time calculations"""
    J2000 = 2451545.0
    
    def __init__(self, jd: float):
        self.jd = jd
        self.mjd = jd - 2400000.5
    
    def __str__(self) -> str:
        return f"JD {self.jd:.6f}"
    
    @staticmethod
    def from_datetime(dt: Union[datetime, DateTime]) -> 'JulianDate':
        if isinstance(dt, DateTime):
            dt = dt.to_datetime()
        year, month = dt.year, dt.month
        day = dt.day + (dt.hour + dt.minute / 60.0 + dt.second / 3600.0) / 24.0
        if month <= 2:
            year -= 1
            month += 12
        A = int(year / 100)
        B = 2 - A + int(A / 4)
        jd = int(365.25 * (year + 4716)) + int(30.6001 * (month + 1)) + day + B - 1524.5
        return JulianDate(jd)
    
    def seconds_since_j2000(self) -> float:
        return (self.jd - self.J2000) * 86400.0

class TimePoint:
    """TimePoint class for precise time representation"""
    def __init__(self, seconds_since_j2000: float):
        self.seconds = seconds_since_j2000
    
    def __str__(self) -> str:
        return f"TimePoint(T={self.seconds:.3f}s since J2000)"
    
    def to_julian_date(self) -> JulianDate:
        return JulianDate(JulianDate.J2000 + self.seconds / 86400.0)
