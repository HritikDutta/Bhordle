#include "physics.h"
#include "object.h"
#include "physics_internal.h"
#include "rigidbody.h"
#include "rigidbody_data.h"
#include "core/logging.h"
#include "containers/darray.h"
#include "math/math.h"
#include "platform/platform.h"
#include "engine/transform.h"

namespace Physics
{

RigidbodyList* bodyList = nullptr;

} // namespace Physics

Rigidbody::Rigidbody()
:   _index((u64) -1)
{
}

Rigidbody::Rigidbody(const Physics::Object& object, f32 mass, const Vector3& velocity)
{
    AssertWithMessage(Physics::bodyList, "Physics was not initialized! Make sure to build with #define GN_USE_PHYSICS.");
    AssertWithMessage(Physics::bodyList->lastBodyIndex < Physics::MAX_OBJECT_COUNT, "Rigidbodies exceeded max count!");

    _index = Physics::bodyList->lastBodyIndex++;

    Physics::bodyList->objects[_index] = object;

    Physics::bodyList->previousPositions[_index] = Physics::bodyList->currentPositions[_index] = object.transform().position();
    Physics::bodyList->velocities[_index] = velocity;
    Physics::bodyList->invMasses[_index] = (mass != 0.0f) ? 1.0f / mass : Math::Infinity;
}

Rigidbody::Rigidbody(const Rigidbody& other)
:   _index(other._index)
{
}

Rigidbody::Rigidbody(Rigidbody&& other)
:   _index(other._index)
{
}

Rigidbody& Rigidbody::operator=(const Rigidbody& other)
{
    _index = other._index;
    return *this;
}

Rigidbody& Rigidbody::operator=(Rigidbody&& other)
{
    _index = other._index;
    return *this;
}

void Rigidbody::Reset(const Vector3& position)
{
    Physics::bodyList->previousPositions[_index] = Physics::bodyList->currentPositions[_index] = Physics::bodyList->objects[_index].transform().position() = position;
    Physics::bodyList->velocities[_index] = Vector3();
    Physics::bodyList->forces[_index] = Vector3();
}

void Rigidbody::SetForce(const Vector3& force)
{
    Physics::bodyList->forces[_index] = force;
}

void Rigidbody::AddForce(const Vector3& force)
{
    Physics::bodyList->forces[_index] += force;
}

void Rigidbody::SetMass(f32 mass)
{
    Physics::bodyList->invMasses[_index] = (mass != 0.0f) ? 1.0f / mass : Math::Infinity;
}

void Rigidbody::SetDynamicFriction(f32 value)
{
    Physics::bodyList->dynamicFrictions[_index] = value;
}

void Rigidbody::SetStaticFriction(f32 value)
{
    Physics::bodyList->staticFrictions[_index] = value;
}

const Physics::Object& Rigidbody::object() const
{
    return Physics::bodyList->objects[_index];
}

Physics::Object& Rigidbody::object()
{
    return Physics::bodyList->objects[_index];
}

const Vector3& Rigidbody::position() const
{
    return object().transform().position();
}

Vector3& Rigidbody::position()
{
    return object().transform().position();
}

const Vector3& Rigidbody::currentPosition() const
{
    return Physics::bodyList->currentPositions[_index];
}

Vector3& Rigidbody::currentPosition()
{
    return Physics::bodyList->currentPositions[_index];
}

const Vector3& Rigidbody::previousPosition() const
{
    return Physics::bodyList->previousPositions[_index];
}

Vector3& Rigidbody::previousPosition()
{
    return Physics::bodyList->previousPositions[_index];
}

const Vector3& Rigidbody::velocity() const
{
    return Physics::bodyList->velocities[_index];
}

Vector3& Rigidbody::velocity()
{
    return Physics::bodyList->velocities[_index];
}

const f32& Rigidbody::restitution() const
{
    return Physics::bodyList->restitutions[_index];
}

f32& Rigidbody::restitution()
{
    return Physics::bodyList->restitutions[_index];
}

f32 Rigidbody::mass() const
{
    f32 invMass = Physics::bodyList->invMasses[_index];
    return (invMass != 0.0f) ? 1.0f / invMass : Math::Infinity;
}

f32 Rigidbody::inverseMass() const
{
    return Physics::bodyList->invMasses[_index];
}