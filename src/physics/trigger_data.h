#pragma once

#include "math/math.h"
#include "engine/transform.h"
#include "collider.h"
#include "object.h"
#include "trigger.h"
#include "physics_internal.h"

namespace Physics
{

enum class TriggerIntersectionState : u32
{
    NONE    = 0,
    EXIT    = 1,
    ENTER   = 2,
    STAY    = 3
};

struct TriggerList
{
    u64 lastTriggerIndex;
    Transform transforms  [MAX_TRIGGER_COUNT];
    CollisionShape shapes [MAX_TRIGGER_COUNT];
    Collider colliders    [MAX_TRIGGER_COUNT];

    // Callbacks
    TriggerCallback triggerEnterCallbacks [MAX_TRIGGER_COUNT];
    TriggerCallback triggerStayCallbacks  [MAX_TRIGGER_COUNT];
    TriggerCallback triggerExitCallbacks  [MAX_TRIGGER_COUNT];
};

TriggerIntersectionState CheckTriggerIntersection(const Trigger& trigger, const Object& object);
void UpdateTriggerStates();

} // namespace Physics
