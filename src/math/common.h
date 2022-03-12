#pragma once

#include <cmath>
#include <ctime>
#include "core/types.h"

template<typename T>
inline T Abs(const T& a)
{
    return (a >= 0) ? a : -a;
}

// Max and Min Functions

template<typename T>
inline T Min(const T& a, const T& b)
{
    return ((a < b) ? a : b);
}

template<typename T>
inline T Max(const T& a, const T& b)
{
    return ((a > b) ? a : b);
}

template<typename T>
inline T Clamp(const T& x, const T& min, const T& max)
{
    return Min(Max(x, min), max);
}

namespace Math
{

// Math Functions

inline f32 Sin(f32 t)
{
    return sinf(t);
}

inline f32 Cos(f32 t)
{
    return cosf(t);
}

inline f32 Tan(f32 t)
{
    return tanf(t);
}

inline f32 Exp(f32 x)
{
    return expf(x);
}

inline f32 Log(f32 x)
{
    return logf(x);
}

inline f32 Floor(f32 x)
{
    return floorf(x);
}

inline f32 Ceil(f32 x)
{
    return ceilf(x);
}

// Extra Functions

// Gives a random float in the range [0, 1)
inline f32 Random()
{
    return rand() / (f32) RAND_MAX;
}

// TODO: Add more functions as needed...

} // namespace Math