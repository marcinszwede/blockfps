#include "TrailManager.h"
#include <d3dx9tex.h>    // D3DXCreateTextureFromFileW
#include <chrono>
#include <cassert>

TrailManager::TrailManager(IDirect3DDevice9* dev)
    : device(dev), texture(nullptr), sprite(nullptr),
    lifetime(1.5f), accumulator(0.0f)
{
    assert(device);
    LoadTexture();
    HRESULT hr = D3DXCreateSprite(device, &sprite);
    assert(SUCCEEDED(hr));
}

TrailManager::~TrailManager() {
    if (texture) texture->Release();
    if (sprite)  sprite->Release();
}

void TrailManager::LoadTexture() {
    HRESULT hr = D3DXCreateTextureFromFileW(
        device,
        L"assets/textures/trail.dds",
        &texture
    );
    assert(SUCCEEDED(hr));
}

void TrailManager::AddPoint(const D3DXVECTOR3& pos) {
    using namespace std::chrono;
    static auto last = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    float dt = duration<float>(now - last).count();
    last = now;

    accumulator += dt;
    if (accumulator > 0.02f) {
        trail.push_back({ pos, 0.0f });
        accumulator = 0.0f;
    }
    for (auto& p : trail) {
        p.age += dt;
    }
    while (!trail.empty() && trail.front().age > lifetime) {
        trail.pop_front();
    }
}

void TrailManager::Render(const D3DXMATRIX& view, const D3DXMATRIX& proj) {
    assert(device && sprite && texture);
    device->SetTransform(D3DTS_VIEW, &view);
    device->SetTransform(D3DTS_WORLD, &view);

    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_BILLBOARD);
    for (const auto& p : trail) {
        float fade = 1.0f - (p.age / lifetime);
        // ZAMIANA NA ¯Ó£TY KOLOR:
        D3DXCOLOR col(1.0f, 1.0f, 0.0f, fade);
        sprite->Draw(texture, nullptr, nullptr, &p.pos, col);
    }
    sprite->End();

    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
