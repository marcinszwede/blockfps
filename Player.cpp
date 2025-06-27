#include "Player.h"
#include "Scene.h"
#include <algorithm>

Player::Player(IDirect3DDevice9* dev) : device(dev) {}

void Player::SetScene(Scene* s) { scene = s; }

void Player::Update(float dt) {
    // 1) Obrót kamery (mouse look)
    yaw += InputManager::GetMouseDX();
    pitch += InputManager::GetMouseDY();
    pitch = std::clamp(pitch, -1.3f, 1.3f);

    D3DXVECTOR3 forward{
        cosf(pitch) * sinf(yaw),
        sinf(pitch),
        cosf(pitch) * cosf(yaw)
    };
    D3DXVECTOR3 right{
        sinf(yaw - D3DX_PI / 2), 0, cosf(yaw - D3DX_PI / 2)
    };

    // 2) Ruch i grawitacja
    D3DXVECTOR3 accel{ 0,-9.81f,0 };
    if (InputManager::IsKeyDown(DIK_W)) accel += forward * 20;
    if (InputManager::IsKeyDown(DIK_S)) accel -= forward * 20;
    if (InputManager::IsKeyDown(DIK_A)) accel -= right * 20;
    if (InputManager::IsKeyDown(DIK_D)) accel += right * 20;
    if (InputManager::IsKeyDown(DIK_SPACE) && onGround) {
        velocity.y = 8.0f;
        onGround = false;
    }

    // 3) Integracja
    velocity += accel * dt;
    velocity.x *= 0.99f; velocity.z *= 0.99f;
    D3DXVECTOR3 next = position + velocity * dt;

    // 4) Kolizje
    if (scene && scene->CheckCollision(next, position)) {
        velocity.y = 0;
        onGround = true;
    }
    else {
        position = next;
        onGround = false;
    }
}

D3DXMATRIX Player::GetViewMatrix() const {
    D3DXMATRIX R, T;
    D3DXMatrixRotationYawPitchRoll(&R, yaw, pitch, 0);
    D3DXMatrixTranslation(&T, -position.x, -position.y, -position.z);
    return R * T;
}

D3DXVECTOR3 Player::GetCameraPosition() const { return position; }

D3DXVECTOR3 Player::GetLookDirection() const {
    return {
        cosf(pitch) * sinf(yaw),
        sinf(pitch),
        cosf(pitch) * cosf(yaw)
    };
}

float Player::GetHeight() const { return position.y; }
