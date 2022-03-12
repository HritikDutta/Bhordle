#include "camera.h"

#include "math/math.h"

Camera Camera::Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
    Camera c;
    c.projection = Matrix4::Orthographic(left, right, bottom, top, near, far);
    return c;
}

Matrix4 Camera::LookAtMatrix(const Vector3& center, const Vector3& up) const
{
    return Matrix4::LookAt(position, center, up);
}