#pragma once

#include "physics.h"
#include "engine/imgui.h"
#include "engine/camera.h"
#include "object_data.h"
#include "rigidbody_data.h"
#include "trigger_data.h"

namespace Physics
{

extern RigidbodyList* bodyList;
extern TriggerList* triggerList;

static void RenderColliders(const Application& app, const Camera& camera, const Matrix4& viewProjection, const Transform& transform, const Physics::Collider& collider, const Vector4& color)
{
    f32 screenWidth = app.window.refWidth;
    f32 screenHeight = app.window.refHeight;

    const Vector4& bounds = collider.aabb.bounds;

    Imgui::Rect rect;
    Vector3 imagePosition = Vector3(transform.position().x + bounds.x, transform.position().y + bounds.y, -0.1f);
    rect.topLeft = (viewProjection * imagePosition + Vector3(1, -1, 0.0f));
    rect.topLeft.y = -rect.topLeft.y;
    rect.topLeft *= 0.5f * Vector3(screenWidth, screenHeight, 1);

    Vector3 size = Vector3(collider.aabb.bounds.z - collider.aabb.bounds.x,
                           collider.aabb.bounds.y - collider.aabb.bounds.w,
                           0.0f);

    size = camera.projection * size;
    size *= 0.5f * Vector3(screenWidth, screenHeight, 1);
    rect.size = Vector2(size.x, size.y);
    
    Imgui::RenderRect(rect, color);
}

void RenderColliders(const Application& app, const Camera& camera)
{
    #ifndef GN_RELEASE

    Imgui::Begin();

    Vector3 center = Vector3(camera.position.x, camera.position.y, 0.0f);
    Matrix4 viewProjection = camera.projection * camera.LookAtMatrix(center);

    // Render Colliders
    for (u64 i = 0; i < bodyList->lastBodyIndex; i++)
        RenderColliders(app, camera, viewProjection, bodyList->objects[i].transform(), bodyList->objects[i].collider(), Vector4(0, 1, 0, 0.25f));

    // Render Triggers
    for (u64 i = 0; i < triggerList->lastTriggerIndex; i++)
        RenderColliders(app, camera, viewProjection, triggerList->transforms[i], triggerList->colliders[i], Vector4(0, 0, 1, 0.25f));

    Imgui::End();

    #endif // GN_RELEASE
}

} // namespace Physics
