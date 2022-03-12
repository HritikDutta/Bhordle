#pragma once

#include "core/types.h"

namespace Math
{
    // Exponential
    constexpr f32 E    = 2.71828182845f;    // e

    // Angles
    constexpr f32 PI   = 3.14159265359f;    // pi
    constexpr f32 PI_2 = 1.57079632679f;    // pi/2
    
    constexpr f32 DegToRad = 0.01745329251f;
    constexpr f32 RadToDeg = 57.2957795131f;

#ifdef _MSC_VER
    static const s8 infBytes[] = { (s8) 0x00, (s8) 0x00, (s8) 0x80, (s8) 0x7F };
    static const f32 Infinity = *(f32*)(infBytes);
#endif

#ifdef __GNUC__
    constexpr f32 Infinity = __builtin_inf();
#endif

} // namespace Math
