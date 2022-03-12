#pragma once

#include "input.h"
#include "input_processing.h"
#include "types.h"
#include "application_internal.h"
#include "platform/platform.h"
#include "containers/darray.h"

struct KeyboardState
{
    bool keys[(int) Key::NUM_KEYS];
};

struct MouseState
{
    s32 x, y;
    s32 mouseWheel;
    bool buttons[(int) MouseButton::NUM_BUTTONS];
};

struct InputState
{
    KeyboardState keyboardState;
    MouseState mouseState;
};

struct InputEvents
{
    DynamicArray<KeyDownCallback> keyDownCallbacks;
};

static InputState currentInputState  = {};
static InputState previousInputState = {};
static InputEvents inputEvents;

inline void InputStateUpdate()
{
    PlatformCopyMemory(&previousInputState, &currentInputState, sizeof(InputState));
}

inline void InputProcessKey(Key key, bool pressed)
{
    if (pressed && !currentInputState.keyboardState.keys[(int) key])
    {
        for (int i = 0; i < inputEvents.keyDownCallbacks.size(); i++)
            inputEvents.keyDownCallbacks[i](GetActiveApplication(), key);
    }

    currentInputState.keyboardState.keys[(int) key] = pressed;
}

inline void InputProcessMouseButton(MouseButton btn, bool pressed)
{
    currentInputState.mouseState.buttons[(int) btn] = pressed;
}

inline void InputProcessMouseMove(s32 x, s32 y)
{
    currentInputState.mouseState.x = x;
    currentInputState.mouseState.y = y;
}

inline void InputProcessMouseWheel(s32 z)
{
    currentInputState.mouseState.mouseWheel = z;
}

// Implementations for input.h

bool Input::GetKey(Key key)
{
    return currentInputState.keyboardState.keys[(int) key];
}

bool Input::GetKeyDown(Key key)
{
    return currentInputState.keyboardState.keys[(int) key] &&
           !previousInputState.keyboardState.keys[(int) key];
}

bool Input::GetKeyUp(Key key)
{
    return !currentInputState.keyboardState.keys[(int) key] &&
           previousInputState.keyboardState.keys[(int) key];
}

bool Input::GetMouseButton(MouseButton button)
{
    return currentInputState.mouseState.buttons[(int) button];
}

bool Input::GetMouseButtonDown(MouseButton button)
{
    return currentInputState.mouseState.buttons[(int) button] &&
           !previousInputState.mouseState.buttons[(int) button];
}

bool Input::GetMouseButtonUp(MouseButton button)
{
    return !currentInputState.mouseState.buttons[(int) button] &&
           previousInputState.mouseState.buttons[(int) button];
}

Vector2 Input::MousePosition()
{
    return Vector2(currentInputState.mouseState.x, currentInputState.mouseState.y);
}

Vector2 Input::DeltaMousePosition()
{
    return Vector2(currentInputState.mouseState.x - previousInputState.mouseState.x,
                   currentInputState.mouseState.y - previousInputState.mouseState.y);
}

void Input::RegisterKeyDownEventCallback(KeyDownCallback onEvent)
{
    inputEvents.keyDownCallbacks.EmplaceBack(onEvent);
}