#pragma once

#include "core/types.h"
#include "math/math.h"

class Transform
{
public:
    Transform();
    Transform(const Vector3& position);
    Transform(const Transform& transform);
    Transform(Transform&& transform);

    Transform& operator=(const Transform& other);
    Transform& operator=(Transform&& other);

    const Vector3& position() const;
    Vector3& position();

    Matrix4 transformMatrix() const;

    u64 index() const { return _index; }

private:
    u64 _index;
};