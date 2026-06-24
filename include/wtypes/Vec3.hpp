#pragma once
// lots of godot similarities here
#include "math/math_common.hpp"
#include <cmath>
#include <format>
#include <iostream>

// GOING OFF A Z up b/c i know blender best
struct Vec3{
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  constexpr Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {};


  static const Vec3 ZERO;
  static const Vec3 LEFT;
  static const Vec3 RIGHT;
  static const Vec3 UP;
  static const Vec3 DOWN;
  static const Vec3 FORWARD;
  static const Vec3 BACK;

  float length() const {
    return std::sqrt(x * x + y * y + z*z);
  }
  
  void zero(){
    x=0;
    y=0;
    z=0;
  }

  float squareLen() const{
    float x2 = x*x;
    float y2 = y*y;
    float z2 = z*z;
    return x2+y2+z2;
  }
  // scale to 1, so 2d is sqrt x^2 + y^2) -> (x/ , y/ )
  void normalize() {
    float len = length();  // because length  is sqrt(x^2 + y^2)
    if (len == 0) {
      zero();
      return;
    }
    x /= len;
    y /= len;
    z /= len;
  }
  Vec3 normalized() const {
    Vec3 n = *this;  // const requires no modification, so this forms a copy
    n.normalize();
    return n;
  }
  // float len = length();  // because length  is sqrt(x^2 + y^2)
  // if (len == 0.0f) return ZERO;
  // return { x / len, y / len };


  float dot(const Vec3& other) const {
    return (x * other.x) + (y * other.y) + (z * other.z);
  }

 Vec3 cross(const Vec3& other) const { 
    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    }; 
  }

  float distance_to(const Vec3& other) const { return (other - *this).length(); }
  float distance_squared_to(const Vec3& other) const { return (other - *this).squareLen(); }
  Vec3 direction_to(const Vec3& other) const { return (other - *this).normalized(); }

  float angle_to(const Vec3& other) const {
    float den = length() * other.length();
    if (Math::is_zero_approx(den)) {
        return 0.0f; 
    }
    float d = Math::clamp(dot(other) / den, -1.0f, 1.0f);
    return std::acos(d);
  }

  // who up lerpin they Vector
  // slides towards another by percentage
  Vec3 lerp(const Vec3& to, float t) const { return *this + (to - *this) * t; }


  // overloads

  Vec3& operator+=(const Vec3& other) {
      x += other.x;
      y += other.y;
      z += other.z;
      return *this;
  }
  Vec3& operator-=(const Vec3& other) {
      x -= other.x;
      y -= other.y;
      z -= other.z;
      return *this;
  }
  Vec3& operator*=(float scalar) {
      x *= scalar;
      y *= scalar;
      z *= scalar;
      return *this;
  }

  Vec3 operator+(const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
  Vec3 operator-(const Vec3& other) const { return { x - other.x, y - other.y, z + other.z }; }
  Vec3 operator*(float scalar) const { return { x * scalar, y * scalar, z*scalar }; }
  Vec3& operator/=(float scalar) {
    if (scalar == 0) {
      std::cerr<<" div by zero attempted. nice going moron"<<std::endl;
      return *this; 
    }
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

Vec3 operator-() const {
    return { -x, -y, -z};
}

Vec3 operator/(float scalar) const {
    return { x / scalar, y / scalar, z / scalar };
}

  friend Vec3 operator*(float scalar, const Vec3& vec) {
    return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
  }


  constexpr bool operator==(const Vec3& other) const {
    return x == other.x && y == other.y && z == other.z;
  }
  constexpr bool operator!=(const Vec3& other) const {
    return !(*this == other);
  }


  bool is_equal_approx(const Vec3& other) const {
    return Math::is_zero_approx(x -other.x) && 
      Math::is_zero_approx(y -other.y) && Math::is_zero_approx(z-other.z);
  }
};

template <>
struct std::formatter<Vec3> : std::formatter<string_view> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }
    auto format(const Vec3& v, format_context& ctx) const {
        return std::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
    }
};
static inline constexpr Vec3 ZERO   { 0.0f,  0.0f,  0.0f };
static inline constexpr Vec3 LEFT   {-1.0f,  0.0f,  0.0f };
static inline constexpr Vec3 RIGHT  { 1.0f,  0.0f,  0.0f };
static inline constexpr Vec3 FORWARD{ 0.0f,  1.0f,  0.0f }; // Y positive
static inline constexpr Vec3 BACK   { 0.0f, -1.0f,  0.0f }; // Y negative
static inline constexpr Vec3 UP     { 0.0f,  0.0f,  1.0f }; // Z positive
static inline constexpr Vec3 DOWN   { 0.0f,  0.0f, -1.0f }; // Z negative
