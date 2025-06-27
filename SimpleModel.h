#pragma once
#include <d3d9.h>
#include <d3dx9.h>

/// Prosty wrapper dla ID3DXMesh + transformacje + materiał + tekstura.
class SimpleModel {
public:
    SimpleModel(IDirect3DDevice9* dev);
    ~SimpleModel();

    // Tworzy pojedynczy płaski kwadrat (2 trójkąty) w płaszczyźnie XZ, środek w (0,0,0)
    static SimpleModel CreatePlane(IDirect3DDevice9* dev, float width, float depth);

    void SetPosition(float x, float y, float z);
    void SetRotation(float pitch, float yaw, float roll);
    void SetScale(float s);

    void SetTexture(IDirect3DTexture9* tex);
    void SetAlpha(float a);

    void Render(IDirect3DDevice9* dev) const;
    bool Intersects(const D3DXVECTOR3& point) const;

private:
    struct Vertex {
        D3DXVECTOR3 pos;
        D3DXVECTOR3 normal;
        D3DXVECTOR2 uv;
    };
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

    ID3DXMesh* mesh = nullptr;
    D3DMATERIAL9         material = {};
    IDirect3DTexture9* texture = nullptr;
    D3DXVECTOR3          minB, maxB;
    D3DXMATRIX           world = {};
    D3DXVECTOR3          position = {};
    D3DXVECTOR3          scale = { 1,1,1 };

    IDirect3DDevice9* device;
    D3DXMATRIX           rotation;

    void UpdateWorld();
    void ComputeBounds();
};
