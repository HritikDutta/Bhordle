#include "trigger.h"

#include "core/types.h"
#include "core/logging.h"
#include "containers/function.h"
#include "math/math.h"
#include "engine/transform.h"
#include "platform/platform.h"
#include "physics_internal.h"
#include "collider.h"
#include "object.h"
#include "trigger_data.h"

namespace Physics
{

TriggerList* triggerList = nullptr;

static bool triggerToObjectMapping[2][MAX_TRIGGER_COUNT * MAX_OBJECT_COUNT] = {};
static u32  currentMappingIndex = 0;

static inline TriggerIntersectionState GetIntersectionState(const Trigger& trigger, const Object& object, bool intersected)
{
    // Determining what to return mathematically to reduce branches
    u64 index = trigger.index() * MAX_TRIGGER_COUNT + object.index();
    u32 result = (u32) TriggerIntersectionState::ENTER * (u32) intersected + (u32) triggerToObjectMapping[currentMappingIndex][index];
    triggerToObjectMapping[1 - currentMappingIndex][index] = intersected;
    return (TriggerIntersectionState) result;
}

TriggerIntersectionState CirclevsCircle(const Trigger& trigger, const Object& object)
{
    Vector3 normal = trigger.transform().position() - object.rigidbody().currentPosition();
    normal.z = 0.0f;

    f32 r = trigger.collider().circle.data.w + object.collider().circle.data.w;

    return GetIntersectionState(trigger, object, normal.SqrLength() <= r * r);
}

static inline TriggerIntersectionState AABBvsAABB(const Trigger& trigger, const Object& object)
{
    const Vector3& triggerPos = trigger.transform().position();
    const Vector3& objectPos  = object.rigidbody().currentPosition();

    Vector4 triggerBounds = trigger.collider().aabb.bounds + Vector4(triggerPos.x, triggerPos.y, triggerPos.x, triggerPos.y);
    Vector4 objectBounds  = object.collider().aabb.bounds  + Vector4(objectPos.x, objectPos.y, objectPos.x, objectPos.y);

    Vector3 n = triggerPos - objectPos;
    
    f32 xOverlap = (n.x < 0.0f) ? triggerBounds.z - objectBounds.x : objectBounds.z - triggerBounds.x;
    f32 yOverlap = (n.y < 0.0f) ? triggerBounds.y - objectBounds.w : objectBounds.y - triggerBounds.w;

    // No overlap means no collision
    return GetIntersectionState(trigger, object, xOverlap > 0 && yOverlap > 0);
}

static inline TriggerIntersectionState AABBvsCircle(const Trigger& trigger, const Object& object)
{
    AssertNotImplemented();
    return TriggerIntersectionState::NONE;
}

TriggerIntersectionState CheckTriggerIntersection(const Trigger& trigger, const Object& object)
{
    if (trigger.shape() == CollisionShape::CIRCLE && object.shape() == CollisionShape::CIRCLE)
        return CirclevsCircle(trigger, object);
    
    if (trigger.shape() == CollisionShape::AABB && object.shape() == CollisionShape::AABB)
        return AABBvsAABB(trigger, object);

    AssertNotImplemented();
    return TriggerIntersectionState::NONE;
}

void UpdateTriggerStates()
{
    currentMappingIndex = 1 - currentMappingIndex;
}

Trigger::Trigger()
:   _index((u64) -1)
{
}

Trigger::Trigger(const Transform& transform, CollisionShape shape, const Vector4& data)
{
    AssertWithMessage(triggerList, "Physics was not initialized! Make sure to build with #define GN_USE_PHYSICS.");
    AssertWithMessage(triggerList->lastTriggerIndex < MAX_TRIGGER_COUNT, "Triggers exceeded max count!");

    _index = triggerList->lastTriggerIndex++;

    this->transform() = transform;
    this->shape() = shape;

    switch (shape)
    {
        case CollisionShape::AABB:
        {
            collider().aabb.bounds = data;
        } break;

        case CollisionShape::CIRCLE:
        {
            collider().circle.data = data;
        } break;

        default:
        {
            AssertNotImplemented();
        } break;
    }
}

Trigger::Trigger(const Trigger& other)
:   _index(other._index)
{
}

Trigger::Trigger(Trigger&& other)
:   _index(other._index)
{
}

Trigger& Trigger::operator=(const Trigger& other)
{
    _index = other._index;
    return *this;
}

Trigger& Trigger::operator=(Trigger&& other)
{
    _index = other._index;
    return *this;
}

const Transform& Trigger::transform() const
{
    return triggerList->transforms[_index];
}

Transform& Trigger::transform()
{
    return triggerList->transforms[_index];
}

const CollisionShape& Trigger::shape() const
{
    return triggerList->shapes[_index];
}

CollisionShape& Trigger::shape()
{
    return triggerList->shapes[_index];
}

const Collider& Trigger::collider() const
{
    return triggerList->colliders[_index];
}

Collider& Trigger::collider()
{
    return triggerList->colliders[_index];
}

void Trigger::SetTriggerEnterCallback(TriggerCallback callback)
{
    triggerList->triggerEnterCallbacks[_index] = callback;
}

void Trigger::SetTriggerStayCallback(TriggerCallback callback)
{
    triggerList->triggerStayCallbacks[_index] = callback;
}

void Trigger::SetTriggerExitCallback(TriggerCallback callback)
{
    triggerList->triggerExitCallbacks[_index] = callback;
}

void Trigger::OnTriggerEnter(Physics::Object& object)
{
    if (triggerList->triggerEnterCallbacks[_index])
        triggerList->triggerEnterCallbacks[_index](object);
}

void Trigger::OnTriggerStay(Physics::Object& object)
{
    if (triggerList->triggerStayCallbacks[_index])
        triggerList->triggerStayCallbacks[_index](object);
}

void Trigger::OnTriggerExit(Physics::Object& object)
{
    if (triggerList->triggerExitCallbacks[_index])
        triggerList->triggerExitCallbacks[_index](object);
}

} // namespace Physics