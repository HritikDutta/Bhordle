#pragma once

#include "core/types.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

template <typename VertexType, s32 maxTexCount>
struct BatchData
{
    Shader shader;

    u32 elemCount;
    VertexType* elemVerticesBuffer, *elemVerticesPtr;

    // Texture data
    Texture textures[maxTexCount];
    u32 nextActiveTexSlot;  // Should always be lower than max allowed textures
};