#pragma once
#include <cmath>

enum class Side { SIDE_LEFT, SIDE_RIGHT, SIDE_TOP, SIDE_BOTTOM };

namespace Math {

inline constexpr double SQRT2 = 1.4142135623730950488016887242;
inline constexpr double SQRT3 = 1.7320508075688772935274463415059;
inline constexpr double SQRT12 = 0.7071067811865475244008443621048490;
inline constexpr double SQRT13 = 0.57735026918962576450914878050196;
inline constexpr double LN2 = 0.6931471805599453094172321215;
inline constexpr double TAU = 6.2831853071795864769252867666;
inline constexpr double PI = 3.1415926535897932384626433833;
inline constexpr double E = 2.7182818284590452353602874714;

inline constexpr float EPSILON = 1e-5f;

inline constexpr float deg_to_rad(float deg) { return deg * (PI / 180.0f); }
inline constexpr float rad_to_deg(float rad) { return rad * (180.0f / PI); }


// ?
template <typename T>
inline constexpr T clamp(T value, T min, T max) {
    return (value < min) ? min : (value > max ? max : value);
}

inline constexpr float lerp(float from, float to, float weight) {
    return from + weight * (to - from);
}

inline constexpr bool is_equal_approx(float a, float b, float tolerance = EPSILON) {
    return std::abs(a - b) < tolerance;
}

inline constexpr bool is_zero_approx(float s) {
    return std::abs(s) < EPSILON;
}

}
