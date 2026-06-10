#pragma once

// ===================================================================================================
// -- standard mathmatical constants and conversions
// ===================================================================================================
template <typename T> constexpr T pi = T(3.1415926535897932384626433832795L);
template <typename T> constexpr T pi2 = pi<T>/T(2);
template <typename T> constexpr T twopi = T(2) * pi<T>;

template <typename T> constexpr T euler = T(2.7182818284590452353602874713526L);

template <typename T> constexpr T to_rad(const T& grad)
{
    return T((double)(grad) * (pi<double> / 180.0));
}

template <typename T> constexpr T to_grad(const T& rad)
{
    return T((double)(rad) * (180.0 / pi<double>));
}
// ===================================================================================================