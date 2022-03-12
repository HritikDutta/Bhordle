#pragma once

#include "core/types.h"
#include "containers/darray.h"
#include "containers/string.h"
#include "containers/stringview.h"
#include "graphics/texture.h"
#include "math/math.h"
#include "renderer2d.h"

using AnimationFrame = R2D::Sprite;

struct AnimationGroup;

struct Animation
{
    enum struct LoopType
    {
        NONE,
        CYCLE,
        PING_PONG
    };

    const AnimationGroup* group;  // This a bad way of handling this lol
    String name;    // For debugging
    LoopType loopType;
    f32 frameRate;
    DynamicArray<AnimationFrame> frames;

    const AnimationFrame& GetCurrentFrame(f32 absoluteTime) const;
};

struct AnimationGroup
{
    Texture atlas;
    DynamicArray<Animation> animations;

    void Load(StringView filepath);
    void Free();
};