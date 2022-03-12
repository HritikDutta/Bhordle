#pragma once

namespace Physics
{

struct AABB
{
    // xy = top left
    // zw = bottom right

    Vector4 bounds;
};

struct Circle
{
    // xyz = position   // Irrelevant
    // w = radius
    Vector4 data;
};

union Collider
{
    AABB   aabb;
    Circle circle;
};

enum struct CollisionShape
{
    AABB      = 0x01,
    CIRCLE    = 0x02,
    NUM_TYPES = 0x04
};

} // namespace Physics