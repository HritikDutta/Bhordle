#pragma once

#include "core/types.h"
#include "math/math.h"
#include "engine/transform.h"
#include "collider.h"
#include "rigidbody.h"

namespace Physics
{

class Object
{
public:
    Object();
    Object(const Transform& transform, CollisionShape shape, const Vector4& data);    // TODO: Need a better way to handle data
    Object(const Object& other);
    Object(Object&& other);

    // Operators
    Object& operator=(const Object& other);
    Object& operator=(Object&& other);

    // Getters and Setters
    const Transform& transform() const;
    Transform& transform();

    const CollisionShape& shape() const;
    CollisionShape& shape();

    const Collider& collider() const;
    Collider& collider();

    const Rigidbody& rigidbody() const;
    Rigidbody& rigidbody();

    u64 index() const { return _index; }

private:
    u64 _index;

    friend void Simulate(f32 timestep);
};

} // namespace Physics
