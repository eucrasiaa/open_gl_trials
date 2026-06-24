#pragma once
// lots of godot similarities here
#include "math/math_common.hpp"
#include <cmath>
#include <format>
#include <iostream>
struct Vec2{
  float x = 0.0f;
  float y = 0.0f;
  constexpr Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {};


  static const Vec2 ZERO;
  static const Vec2 LEFT;
  static const Vec2 RIGHT;
  static const Vec2 UP;
  static const Vec2 DOWN;

  /**
   * length, calculated by x^2 + y^2
   *
   *
   */
  float length() const {
    return std::sqrt(x * x + y * y);
  }

  void zero(){
    x=0;
    y=0;
  }

  float squareLen() const{
    float x2 = x*x;
    float y2 = y*y;
    return x2+y2;
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
  }
  Vec2 normalized() const {
    Vec2 n = *this;  // const requires no modification, so this forms a copy
    n.normalize();
    return n;
  }
  // float len = length();  // because length  is sqrt(x^2 + y^2)
  // if (len == 0.0f) return ZERO;
  // return { x / len, y / len };


  float dot(const Vec2& other) const {
    return (x * other.x) + (y * other.y);
  }

  float cross(const Vec2& other) const { return x * other.y - y * other.x; }
  float distance_to(const Vec2& other) const { return (other - *this).length(); }
  float distance_squared_to(const Vec2& other) const { return (other - *this).squareLen(); }
  Vec2 direction_to(const Vec2& other) const { return (other - *this).normalized(); }
  float angle() const { return std::atan2(y, x); }
  float angle_to(const Vec2& other) const { return std::atan2(cross(other), dot(other)); }

  // who up lerpin they Vector
  // slides towards another by percentage
  Vec2 lerp(const Vec2& to, float t) const { return *this + (to - *this) * t; }
  Vec2 perpendicular() const { return { -y, x }; }


  // overloads

  Vec2& operator+=(const Vec2& other) {
    x += other.x;
    y += other.y;
    return *this;
  }
  Vec2& operator-=(const Vec2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }
  Vec2& operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  Vec2 operator+(const Vec2& other) const { return { x + other.x, y + other.y }; }
  Vec2 operator-(const Vec2& other) const { return { x - other.x, y - other.y }; }
  Vec2 operator*(float scalar) const { return { x * scalar, y * scalar }; }
  Vec2& operator/=(float scalar) {
    if (scalar == 0) {
      std::cerr<<" div by zero attempted. nice going moron"<<std::endl;
      return *this; 
    }
    x /= scalar;
    y /= scalar;
    return *this;
  }
  Vec2 operator-() const {
    return { -x, -y };
  }


  Vec2 operator/(float scalar) const {
    return { x / scalar, y / scalar };
  }

  friend Vec2 operator*(float scalar, const Vec2& vec) {
    return { vec.x * scalar, vec.y * scalar };
  }


  constexpr bool operator==(const Vec2& other) const {
    return x == other.x && y == other.y;
  }
  constexpr bool operator!=(const Vec2& other) const {
    return !(*this == other);
  }


  bool is_equal_approx(const Vec2& other) const {
    return Math::is_zero_approx(x -other.x) && 
      Math::is_zero_approx(y -other.y);
  }
};

template <>
struct std::formatter<Vec2> : std::formatter<string_view> {
  constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }
  auto format(const Vec2& v, format_context& ctx) const {
    return std::format_to(ctx.out(), "({}, {})", v.x, v.y);
  }
};
// as per Vec3, i doing a Z up. so left right on x
inline constexpr Vec2 Vec2::ZERO   { 0.0f,  0.0f };
inline constexpr Vec2 LEFT   {-1.0f,  0.0f };
inline constexpr Vec2 RIGHT  { 1.0f,  0.0f };
inline constexpr Vec2 UP     { 0.0f,  1.0f };
inline constexpr Vec2 DOWN   { 0.0f,  -1.0f };


