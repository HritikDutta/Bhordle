#pragma once

#include "core/types.h"
#include "math/math.h"

namespace Physics
{
class Object;
}

class Rigidbody
{
public:
    Rigidbody();
    Rigidbody(const Physics::Object& object, f32 mass = Math::Infinity, const Vector3& velocity = Vector3());
    Rigidbody(const Rigidbody& other);
    Rigidbody(Rigidbody&& other);

    // Operators
    Rigidbody& operator=(const Rigidbody& other);
    Rigidbody& operator=(Rigidbody&& other);
    
    // Interface Functions
    void Reset(const Vector3& position = Vector3());
    void SetForce(const Vector3& force);
    void AddForce(const Vector3& force);

    // Getters and Setters
    const Physics::Object& object() const;
    Physics::Object& object();

    const Vector3& position() const;
    Vector3& position();

    const Vector3& currentPosition() const;
    Vector3& currentPosition();

    const Vector3& previousPosition() const;
    Vector3& previousPosition();

    const Vector3& velocity() const;
    Vector3& velocity();

    const f32& restitution() const;
    f32& restitution();

    // Only Getters
    f32 mass() const;
    f32 inverseMass() const;

    u64 index() const { return _index; }

    // Only Setters
    void SetMass(f32 mass);
    void SetDynamicFriction(f32 value);
    void SetStaticFriction(f32 value);

private:
    u64 _index;
};