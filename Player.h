#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "InputManager.h"
class Scene;

class Player {
public:
    Player(IDirect3DDevice9* dev);
    void SetScene(Scene* s);
    void Update(float dt);

    D3DXMATRIX   GetViewMatrix()     const;
    D3DXVECTOR3  GetCameraPosition() const;
    D3DXVECTOR3  GetLookDirection()  const;
    float        GetHeight()         const;

private:
    IDirect3DDevice9* device;
    Scene* scene = nullptr;

    D3DXVECTOR3 position{ 0,5,0 };
    D3DXVECTOR3 velocity{ 0,0,0 };
    float        yaw = 0, pitch = 0;
    bool         onGround = false;
};
