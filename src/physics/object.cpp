#include "object.h"

#include "core/types.h"
#include "core/logging.h"
#include "math/math.h"
#include "engine/transform.h"
#include "physics_internal.h"
#include "rigidbody.h"
#include "collider.h"
#include "object_data.h"

namespace Physics
{

ObjectList* objectList = nullptr;

static inline bool CirclevsCircle(CollisionData& data)
{
    Object& a = data.a;
    Object& b = data.b;

    Vector3 normal = b.rigidbody().currentPosition() - a.rigidbody().currentPosition();
    normal.z = 0.0f;

    f32 r = a.collider().circle.data.w + b.collider().circle.data.w;

    // Return if circles are not colliding
    if (normal.SqrLength() > r * r)
        return false;
    
    f32 dist = normal.Length();

    if (dist != 0)
    {
        // Penetration is the totalRadius - distance
        data.penetrationDepth = r - dist;
        data.collisionNormal = normal / dist;
    }
    else
    {
        // Circles are on the same position
        data.penetrationDepth = a.collider().circle.data.w; // Can be any radius
        data.collisionNormal = Vector3(1, 0, 0);
    }

    return true;
}

static inline bool AABBvsAABB(CollisionData& data)
{
    const Object& a = data.a;
    const Object& b = data.b;

    const Vector3& aPosition = a.rigidbody().currentPosition();
    const Vector3& bPosition = b.rigidbody().currentPosition();

    Vector4 aBounds = a.collider().aabb.bounds + Vector4(aPosition.x, aPosition.y, aPosition.x, aPosition.y);
    Vector4 bBounds = b.collider().aabb.bounds + Vector4(bPosition.x, bPosition.y, bPosition.x, bPosition.y);

    Vector3 n = bPosition - aPosition;

    // No overlap means no collision
    f32 xOverlap = (n.x < 0.0f) ? bBounds.z - aBounds.x : aBounds.z - bBounds.x;
    if (xOverlap < 0)
        return false;

    // No overlap means no collision
    f32 yOverlap = (n.y < 0.0f) ? bBounds.y - aBounds.w : aBounds.y - bBounds.w;
    if (yOverlap < 0)
        return false;

    // Find out which axis has the least penetration
    if (xOverlap < yOverlap)
    {
        data.collisionNormal = Vector3((n.x < 0) ? -1 : 1 , 0, 0);
        data.penetrationDepth = xOverlap;
    }
    else
    {
        data.collisionNormal = Vector3(0, (n.y < 0) ? -1 : 1, 0);
        data.penetrationDepth = yOverlap;
    }

    return true;
}

static inline bool AABBvsCircle(CollisionData& data)
{
    AssertNotImplemented();
    return false;
}

bool EvaluateCollision(CollisionData& data)
{
    // TODO: There must be a better way to do this
    
    if (data.a.shape() == CollisionShape::CIRCLE && data.b.shape() == CollisionShape::CIRCLE)
        return CirclevsCircle(data);
    
    if (data.a.shape() == CollisionShape::AABB && data.b.shape() == CollisionShape::AABB)
        return AABBvsAABB(data);

    AssertNotImplemented();    
    return false;
}

void ResolveCollision(CollisionData& data)
{
    Object& a = data.a;
    Object& b = data.b;

    Vector3 relativeVelocity = b.rigidbody().velocity() - a.rigidbody().velocity();
    f32 velAlongNormal = Dot(relativeVelocity, data.collisionNormal);

    // Don't resolve if bodies aren't moving towards each other
    if (velAlongNormal > 0.0f)
        return;
    
    Rigidbody& aRB = a.rigidbody();
    Rigidbody& bRB = b.rigidbody();
    
    // Calculate impulse scalar
    f32 e = Min(aRB.restitution(), bRB.restitution());
    f32 j = -(1 + e) * velAlongNormal;
    j /= aRB.inverseMass() + bRB.inverseMass();

    // Calulcate impulse vector
    Vector3 impulse = j * data.collisionNormal;

    f32 massOfA = aRB.mass();
    f32 massOfB = bRB.mass();
    f32 totalMass = massOfA + massOfB;

    // Apply impulse according to mass;
    aRB.velocity() -= aRB.inverseMass() * impulse;
    bRB.velocity() += bRB.inverseMass() * impulse;

    Vector3 correction = (Max(data.penetrationDepth - 0.01f, 0.0f) / (aRB.inverseMass() + bRB.inverseMass()) * 0.2f) * data.collisionNormal;
    aRB.currentPosition() -= aRB.inverseMass() * correction;
    bRB.currentPosition() += bRB.inverseMass() * correction;
}

Object::Object()
:   _index((u64) -1)
{
}

Object::Object(const Transform& transform, CollisionShape shape, const Vector4& data)
{
    AssertWithMessage(objectList, "Physics was not initialized! Make sure to build with #define GN_USE_PHYSICS.");
    AssertWithMessage(objectList->lastObjectIndex < MAX_OBJECT_COUNT, "Physics Objects exceeded max count!");
    
    _index = objectList->lastObjectIndex++;

    this->transform() = transform;
    this->rigidbody() = Rigidbody(*this);

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

Object::Object(const Object& other)
:   _index(other._index)
{
}

Object::Object(Object&& other)
:   _index(other._index)
{
}

Object& Object::operator=(const Object& other)
{
    _index = other._index;
    return *this;
}

Object& Object::operator=(Object&& other)
{
    _index = other._index;
    return *this;
}

const Transform& Object::transform() const
{
    return objectList->transforms[_index];
}

Transform& Object::transform()
{
    return objectList->transforms[_index];
}

const CollisionShape& Object::shape() const
{
    return objectList->shapes[_index];
}

CollisionShape& Object::shape()
{
    return objectList->shapes[_index];
}

const Collider& Object::collider() const
{
    return objectList->colliders[_index];
}

Collider& Object::collider()
{
    return objectList->colliders[_index];
}

const Rigidbody& Object::rigidbody() const
{
    return objectList->rigidbodies[_index];
}

Rigidbody& Object::rigidbody()
{
    return objectList->rigidbodies[_index];
}

} // namespace Physics
