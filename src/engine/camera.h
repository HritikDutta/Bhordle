#pragma once

#include "math/math.h"

struct Camera
{
    Matrix4 projection;
    Vector3 position;

    static Camera Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

    Matrix4 LookAtMatrix(const Vector3& center, const Vector3& up = Vector3::up) const;
};