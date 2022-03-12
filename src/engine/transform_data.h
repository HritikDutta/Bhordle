#pragma once

static constexpr u64 MAX_TRANSFORMS = 128;

struct TransformList
{
    u64 lastBodyIndex;
    Vector3 positions[MAX_TRANSFORMS];
};
