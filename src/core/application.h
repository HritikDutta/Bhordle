#pragma once

#include "containers/string.h"
#include "containers/function.h"
#include "types.h"

struct WindowData
{
    String name;
    String iconPath;
    s32 x, y;
    s32 width, height;
    s32 refHeight, refWidth;
};

struct Application
{
    WindowData window;
    void* data = nullptr;

    f32 time = 0.0f;
    f32 deltaTime = 0.0f;

    Function<void(Application& app)> OnInit     = [](Application&) {};
    Function<void(Application& app)> OnUpdate   = [](Application&) {};
    Function<void(Application& app)> OnRender   = [](Application&) {};
    Function<void(Application& app)> OnShutdown = [](Application&) {};
    Function<void(Application& app)> OnWindowResize = [](Application&) {};

    void Exit();
};