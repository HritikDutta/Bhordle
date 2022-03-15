#include "application.h"
#include "application_internal.h"
#include "input.h"
#include "input_processing.h"
#include "engine/engine.h"
#include "graphics/graphics.h"
#include "platform/platform.h"
#include "physics/physics.h"

// For random numbers
#include <ctime>

extern void CreateApp(Application& app);

int main()
{
    // For Random Numbers
    srand(time(0));

    Application app;
    CreateApp(app);

    app.window.refHeight = app.window.height;
    app.window.refWidth = app.window.width;

    SetActiveApplication(&app);

    PlatformState pstate;
    if (!PlatformWindowStartup(pstate,
        app.window.name.cstr(),
        app.window.x, app.window.y,
        app.window.width, app.window.height,
        app.window.iconPath.cstr()))
    {
        return 1;
    }

    GraphicsSetVsync(true);

    Engine::Init(app);

    #ifdef GN_USE_PHYSICS
    Physics::Init();
    #endif

    app.OnInit(app);

    f32 prevTime = PlatformGetTime();

    while (IsApplicationRunning())
    {
        app.time = PlatformGetTime();
        app.deltaTime = app.time - prevTime;
        prevTime = app.time;

        PlatformPumpMessages();
        GraphicsClearCanvas();

        app.OnUpdate(app);

        #ifdef GN_USE_PHYSICS
        Physics::Simulate(app.deltaTime);
        #endif

        app.OnRender(app);

        GraphicsSwapBuffers(pstate);
        InputStateUpdate();
    }

    app.OnShutdown(app);

    #ifdef GN_USE_PHYSICS
    Physics::Shutdown();
    #endif

    Engine::Shutdown();

    PlatformWindowShutdown(pstate);
}