#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <wrl/client.h>
#include "SimpleModel.h"

using Microsoft::WRL::ComPtr;

/// Generuje i trzyma w sobie siatkê kostek + ground plane.
class Maze {
public:
    Maze(IDirect3DDevice9* dev, int w, int h);
    ~Maze();

    void Render();
    int  HitTest(const D3DXVECTOR3& point) const;

private:
    void LoadTextures();
    void BuildMeshes();

    IDirect3DDevice9* device;
    int                     width, height;
    std::vector<SimpleModel> cubes;
    SimpleModel             groundPlane;

    ComPtr<IDirect3DTexture9> cubeTex;
    ComPtr<IDirect3DTexture9> groundTex;
};
