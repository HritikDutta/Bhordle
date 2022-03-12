#pragma once

#include "core/types.h"
#include "core/application.h"
#include "engine/camera.h"

namespace Physics
{

static constexpr f32 TIMESTEP = 1.0f / 144.0f;
static constexpr f32 MAX_DELTA_TIME = 0.2f;

void Init();
void Shutdown();

void Simulate(f32 deltaTime);

void RenderColliders(const Application& app, const Camera& camera);

} // namespace Physics
