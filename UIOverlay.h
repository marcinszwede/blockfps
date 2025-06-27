#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <cfloat>         // FLT_MAX
#include "Player.h"
#include "Scene.h"

class UIOverlay {
public:
    UIOverlay(IDirect3DDevice9* dev);
    ~UIOverlay();

    void Render(const Player& player, const Scene& scene);

private:
    IDirect3DDevice9* device;
    ID3DXFont* font;
    ID3DXSprite* sprite;
    float             elapsed = 0.0f;
    float             best = FLT_MAX;
};
