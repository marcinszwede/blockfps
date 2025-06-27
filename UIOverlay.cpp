#include "UIOverlay.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <cfloat>
#include <string>
#include <chrono>
#include <algorithm>

UIOverlay::UIOverlay(IDirect3DDevice9* dev)
    : device(dev), font(nullptr), sprite(nullptr), elapsed(0.0f), best(FLT_MAX)
{
    HRESULT hr = D3DXCreateFontW(
        device,
        16, 0, FW_BOLD, 1, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"Arial",
        &font
    );
    if (FAILED(hr)) font = nullptr;

    hr = D3DXCreateSprite(device, &sprite);
    if (FAILED(hr)) sprite = nullptr;
}

UIOverlay::~UIOverlay() {
    if (font)   font->Release();
    if (sprite) sprite->Release();
}

void UIOverlay::Render(const Player& player, const Scene&) {
    if (!font || !sprite) return;

    using namespace std::chrono;
    static auto last = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    float dt = duration<float>(now - last).count();
    last = now;
    elapsed += dt;

    if (player.GetHeight() < 0.1f) {
        best = std::min(best, elapsed);
        elapsed = 0.0f;
    }

    sprite->Begin(D3DXSPRITE_ALPHABLEND);
    RECT r = { 10, 10, 0, 0 };
    std::wstring txt;

    txt = L"FPS: " + std::to_wstring(int(1.0f / dt));
    font->DrawTextW(sprite, txt.c_str(), -1, &r, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));

    r.top += 20;
    txt = L"Time: " + std::to_wstring(int(elapsed)) + L"s";
    font->DrawTextW(sprite, txt.c_str(), -1, &r, DT_NOCLIP, D3DCOLOR_XRGB(200, 200, 255));

    r.top += 20;
    txt = L"Best: " + ((best < FLT_MAX)
        ? std::to_wstring(int(best)) + L"s"
        : L"--");
    font->DrawTextW(sprite, txt.c_str(), -1, &r, DT_NOCLIP, D3DCOLOR_XRGB(200, 255, 200));

    r.top += 20;
    txt = L"Height: " + std::to_wstring(int(player.GetHeight()));
    font->DrawTextW(sprite, txt.c_str(), -1, &r, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 200));

    sprite->End();
}
