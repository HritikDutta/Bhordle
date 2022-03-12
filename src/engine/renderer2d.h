#pragma once

#include "math/math.h"
#include "graphics/texture.h"
#include "camera.h"

namespace R2D
{

void Init();
void Shutdown();

void Begin(const Camera& camera);
void End();

struct Sprite
{
    Texture atlas;
    Vector4 texCoords;
    Vector2 pivot;
};

void RenderSprite(const Sprite& sprite, const Matrix4& transform);

} // namespace R2D
