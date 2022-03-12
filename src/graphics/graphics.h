#pragma once

#include "platform/platform.h"

bool GraphicsInit(InternalState& state);
void GraphicsShutdown(InternalState& state);

void GraphicsSwapBuffers(const PlatformState& pstate);
void GraphicsResizeCanvasCallback(s32 width, s32 height);

void GraphicsSetVsync(bool value);

void GraphicsClearCanvas();