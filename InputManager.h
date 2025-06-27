#pragma once
#include <dinput.h>
#include <Windows.h>

/// Zarządza DirectInput: keyboard + mouse, smoothing myszy.
class InputManager {
public:
    static void Initialize(HWND hWnd, HINSTANCE hInst);
    static void Poll();
    static float GetMouseDX();
    static float GetMouseDY();
    static bool  IsKeyDown(int diKey);
};
