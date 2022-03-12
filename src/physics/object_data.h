#pragma once

#include "math/math.h"
#include "engine/transform.h"
#include "object.h"
#include "collider.h"
#include "rigidbody.h"
#include "physics_internal.h"

namespace Physics
{

struct CollisionData
{
    Object a;
    Object b;
    Vector3 collisionNormal;
    f32 penetrationDepth;
};

struct ObjectList
{
    u64 lastObjectIndex;
    Transform transforms  [MAX_OBJECT_COUNT];
    Rigidbody rigidbodies [MAX_OBJECT_COUNT];
    CollisionShape shapes [MAX_OBJECT_COUNT];
    Collider colliders    [MAX_OBJECT_COUNT];
};

bool EvaluateCollision(CollisionData& data);
void ResolveCollision(CollisionData& data);

} // namespace Physics
