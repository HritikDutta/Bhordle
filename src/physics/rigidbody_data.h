#pragma once

#include "math/math.h"
#include "object.h"
#include "rigidbody.h"
#include "physics_internal.h"

namespace Physics
{

struct RigidbodyList
{
    u64     lastBodyIndex;
    Object  objects           [MAX_OBJECT_COUNT];
    Vector3 currentPositions  [MAX_OBJECT_COUNT];
    Vector3 previousPositions [MAX_OBJECT_COUNT];
    Vector3 velocities        [MAX_OBJECT_COUNT];
    f32     invMasses         [MAX_OBJECT_COUNT];
    Vector3 forces            [MAX_OBJECT_COUNT];

    // TODO: These should be separate material properties as they can be shared among objects
    f32     dynamicFrictions [MAX_OBJECT_COUNT];
    f32     staticFrictions  [MAX_OBJECT_COUNT];
    f32     restitutions     [MAX_OBJECT_COUNT];
};
    
} // namespace Physics