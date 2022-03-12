#include "transform.h"

#include "core/logging.h"
#include "core/types.h"
#include "math/math.h"
#include "transform_data.h"

TransformList* transforms = nullptr;

Transform::Transform()
:   _index((u64) -1)
{
}

Transform::Transform(const Vector3& position)
{
    AssertWithMessage(transforms, "Physics was not initialized! Make sure to build with #define GN_USE_PHYSICS.");
    AssertWithMessage(transforms->lastBodyIndex < MAX_TRANSFORMS, "Rigidbodies increased max count!");

    _index = transforms->lastBodyIndex++;

    transforms->positions[_index] = position;
}

Transform::Transform(const Transform& other)
:   _index(other._index)
{
}

Transform::Transform(Transform&& other)
:   _index(other._index)
{
}

Transform& Transform::operator=(const Transform& other)
{
    _index = other._index;
    return *this;
}

Transform& Transform::operator=(Transform&& other)
{
    _index = other._index;
    return *this;
}

const Vector3& Transform::position() const
{
    return transforms->positions[_index];
}

Vector3& Transform::position()
{
    return transforms->positions[_index];
}

Matrix4 Transform::transformMatrix() const
{
    return Matrix4::Translation(transforms->positions[_index]);
}