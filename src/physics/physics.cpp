#include "physics.h"
#include "rigidbody.h"
#include "core/logging.h"
#include "containers/darray.h"
#include "math/math.h"
#include "platform/platform.h"
#include "object.h"
#include "trigger.h"
#include "object_data.h"
#include "rigidbody_data.h"
#include "trigger_data.h"

namespace Physics
{

extern RigidbodyList* bodyList;
extern ObjectList* objectList;
extern TriggerList* triggerList;

void Init()
{
    bodyList = (RigidbodyList*) PlatformAllocate(sizeof(RigidbodyList));
    AssertWithMessage(bodyList, "Failed to allocate Rigidbody Data!");
    PlatformZeroMemory(bodyList, sizeof(RigidbodyList));

    objectList = (ObjectList*) PlatformAllocate(sizeof(ObjectList));
    AssertWithMessage(objectList, "Failed to allocate Physics Object Data!");
    PlatformZeroMemory(objectList, sizeof(ObjectList));

    triggerList = (TriggerList*) PlatformAllocate(sizeof(TriggerList));
    AssertWithMessage(triggerList, "Failed to allocate Trigger Data!");
    PlatformZeroMemory(triggerList, sizeof(TriggerList));
}

void Shutdown()
{
    AssertWithMessage(bodyList && objectList, "Physics was not initialized!");

    // No need to free object list since it's just one allocation
    PlatformFree(bodyList);
    PlatformFree(objectList);
    PlatformFree(triggerList);
}

void Simulate(f32 deltaTime)
{
    // Make sure physics was initialized
    AssertWithMessage(bodyList && objectList, "Physics was not initialized!");

    static f32 accumulator = 0.0f;
    accumulator += (deltaTime < MAX_DELTA_TIME) ? deltaTime: MAX_DELTA_TIME;    // To prevent too many loops

    while (accumulator >= TIMESTEP)
    {
        // Update all rigidbodies
        for (u64 i = 0; i < bodyList->lastBodyIndex; i++)
        {
            Physics::bodyList->velocities[i] += Physics::bodyList->invMasses[i] * Physics::bodyList->forces[i] * TIMESTEP;

            f32 frictionModifier = Clamp(Physics::bodyList->forces[i].SqrLength(), 0.0f, 1.0f);
            f32 friction = frictionModifier * Physics::bodyList->dynamicFrictions[i] + (1.0f - frictionModifier) * Physics::bodyList->staticFrictions[i];
            Physics::bodyList->velocities[i] -= friction * Physics::bodyList->velocities[i];

            Physics::bodyList->previousPositions[i] = Physics::bodyList->currentPositions[i];
            Physics::bodyList->currentPositions[i] += Physics::bodyList->velocities[i] * TIMESTEP;
        }

        for (u64 i = 0; i < objectList->lastObjectIndex; i++)
        {
            {   // Check for collisions with other objects

                CollisionData data;
                data.a._index = i;

                for (u64 j = i + 1; j < objectList->lastObjectIndex; j++)
                {
                    data.b._index = j;

                    if (EvaluateCollision(data))
                        ResolveCollision(data);
                }

            }

            {   // Check for intersection with triggers

                Object object;
                object._index = i;

                Trigger trigger;
                for (u64 j = 0; j < triggerList->lastTriggerIndex; j++)
                {
                    trigger._index = j;

                    switch (CheckTriggerIntersection(trigger, object))
                    {
                        case TriggerIntersectionState::ENTER:
                        {
                            trigger.OnTriggerEnter(object);
                        } break;
                        
                        case TriggerIntersectionState::STAY:
                        {
                            trigger.OnTriggerStay(object);
                        } break;
                        
                        case TriggerIntersectionState::EXIT:
                        {
                            trigger.OnTriggerExit(object);
                        } break;
                    }
                }
                
            }
        }

        accumulator -= TIMESTEP;
        
        UpdateTriggerStates();
    }

    {   // Interpolate results to reduce jitter in rendering
        f32 alpha = accumulator / TIMESTEP;
        f32 oneMinusAlpha = 1.0f - alpha;

        for (u64 i = 0; i < bodyList->lastBodyIndex; i++)
        {
            Vector3& currentPosition = Physics::bodyList->currentPositions[i];
            Vector3& previousPosition = Physics::bodyList->previousPositions[i];

            Physics::bodyList->objects[i].transform().position() = currentPosition * alpha + previousPosition * oneMinusAlpha;
            previousPosition = currentPosition;
        }
    }
}

} // namespace Physics