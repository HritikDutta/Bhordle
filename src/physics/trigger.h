
#pragma once

#include "core/types.h"
#include "math/math.h"
#include "engine/transform.h"
#include "containers/function.h"
#include "collider.h"
#include "object.h"

namespace Physics
{

using TriggerCallback = Function<void(Object&)>;

class Trigger
{
public:
    Trigger();
    Trigger(const Transform& transform, CollisionShape shape, const Vector4& data);
    Trigger(const Trigger& other);
    Trigger(Trigger&& other);

    // Operators
    Trigger& operator=(const Trigger& other);
    Trigger& operator=(Trigger&& other);
 
    // Getters and Setters
    const Transform& transform() const;
    Transform& transform();

    const CollisionShape& shape() const;
    CollisionShape& shape();

    const Collider& collider() const;
    Collider& collider();

    u64 index() const { return _index; }
    
    void SetTriggerEnterCallback(TriggerCallback callback);
    void SetTriggerStayCallback(TriggerCallback callback);
    void SetTriggerExitCallback(TriggerCallback callback);

    void OnTriggerEnter(Physics::Object& object);
    void OnTriggerStay(Physics::Object& object);
    void OnTriggerExit(Physics::Object& object);

private:
    u64 _index;

    friend void Simulate(f32 timestep);
};

} // namespace Physics
