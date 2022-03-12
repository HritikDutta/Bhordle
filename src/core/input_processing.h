#pragma once

#include "types.h"

extern inline void InputStateUpdate();

extern inline void InputProcessKey(Key key, bool pressed);
extern inline void InputProcessMouseButton(MouseButton btn, bool pressed);
extern inline void InputProcessMouseMove(s32 x, s32 y);
extern inline void InputProcessMouseWheel(s32 z);