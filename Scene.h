#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "Maze.h"
#include "InputManager.h"

class Player;

class Scene {
public:
    Scene(IDirect3DDevice9* device);
    ~Scene();

    void Update(const Player& player, float dt);
    void Render();
    bool CheckCollision(const D3DXVECTOR3& next, const D3DXVECTOR3& prev) const;

private:
    IDirect3DDevice9* device;
    Maze* maze;

    D3DLIGHT9 dirLight{};
    D3DLIGHT9 spotLight{};
    bool      spotEnabled = true;
};
