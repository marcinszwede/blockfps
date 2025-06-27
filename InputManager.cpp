#include "InputManager.h"
#include <array>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

static ComPtr<IDirectInput8>       g_di;
static ComPtr<IDirectInputDevice8> g_keyboard;
static ComPtr<IDirectInputDevice8> g_mouse;
static std::array<BYTE, 256>        g_keyState;
static DIMOUSESTATE2               g_mouseState;

void InputManager::Initialize(HWND hWnd, HINSTANCE hInst) {
    DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
        reinterpret_cast<void**>(g_di.GetAddressOf()), nullptr);
    // klawiatura
    g_di->CreateDevice(GUID_SysKeyboard, g_keyboard.GetAddressOf(), nullptr);
    g_keyboard->SetDataFormat(&c_dfDIKeyboard);
    g_keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    g_keyboard->Acquire();
    // mysz
    g_di->CreateDevice(GUID_SysMouse, g_mouse.GetAddressOf(), nullptr);
    g_mouse->SetDataFormat(&c_dfDIMouse2);
    g_mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    g_mouse->Acquire();
}

void InputManager::Poll() {
    if (g_keyboard) g_keyboard->GetDeviceState(g_keyState.size(), g_keyState.data());
    if (g_mouse)    g_mouse->GetDeviceState(sizeof(g_mouseState), &g_mouseState);
}

float InputManager::GetMouseDX() { return g_mouseState.lX * 0.0025f; }
float InputManager::GetMouseDY() { return g_mouseState.lY * 0.0025f; }

bool InputManager::IsKeyDown(int diKey) {
    return (g_keyState[diKey] & 0x80) != 0;
}
