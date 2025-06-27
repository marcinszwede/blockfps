#include "Maze.h"
#include <d3dx9tex.h>
#include <cassert>

Maze::Maze(IDirect3DDevice9* dev, int w, int h)
    : device(dev), width(w), height(h), groundPlane(dev)
{
    assert(device);
    LoadTextures();
    BuildMeshes();
}

void Maze::LoadTextures() {
    HRESULT hr;
    hr = D3DXCreateTextureFromFileW(device, L"assets/textures/cube.png", cubeTex.GetAddressOf());
    assert(SUCCEEDED(hr));
    hr = D3DXCreateTextureFromFileW(device, L"assets/textures/ground.png", groundTex.GetAddressOf());
    assert(SUCCEEDED(hr));
}

void Maze::BuildMeshes() {
    cubes.clear();
    cubes.reserve(size_t(width) * size_t(height));
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            SimpleModel cube(device);
            cube.SetTexture(cubeTex.Get());
            cube.SetPosition(float(x * 2), 0.0f, float(z * 2));
            cubes.push_back(std::move(cube));
        }
    }
    groundPlane = SimpleModel::CreatePlane(device, float(width * 2), float(height * 2));
    groundPlane.SetTexture(groundTex.Get());
    groundPlane.SetPosition(float(width), -0.5f, float(height));
    groundPlane.SetRotation(-D3DX_PI / 2, 0, 0);
}
