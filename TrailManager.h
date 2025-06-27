#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <deque>

/// Billboardowy œlad za graczem.
class TrailManager {
public:
    TrailManager(IDirect3DDevice9* dev);
    ~TrailManager();
    void AddPoint(const D3DXVECTOR3& pos);
    void Render(const D3DXMATRIX& view, const D3DXMATRIX& proj);

private:
    struct Point { D3DXVECTOR3 pos; float age; };
    IDirect3DDevice9* device;
    IDirect3DTexture9* texture;
    ID3DXSprite* sprite;
    std::deque<Point> trail;
    float lifetime = 1.5f;
    float accumulator = 0;
    void LoadTexture();
};
