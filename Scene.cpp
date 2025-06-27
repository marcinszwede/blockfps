#include "Scene.h"
#include "Player.h"
#include <chrono>

Scene::Scene(IDirect3DDevice9* dev)
    : device(dev)
{
    maze = new Maze(device, 10, 10);
    // ambient + directional light
    dirLight.Type = D3DLIGHT_DIRECTIONAL;
    dirLight.Direction = { -0.5f,-1.0f,0.5f };
    dirLight.Diffuse = dirLight.Specular = D3DXCOLOR(1, 1, 1, 1);
    device->SetLight(0, &dirLight);
    device->LightEnable(0, TRUE);
}

Scene::~Scene() {
    delete maze;
}

void Scene::Update(const Player& player, float dt) {
    // 1) Prze³¹czanie latarki (F)
    static bool lastF = false;
    bool curF = InputManager::IsKeyDown(DIK_F);
    if (curF && !lastF) spotEnabled = !spotEnabled;
    lastF = curF;

    // 2) Ustawienie spotlight
    if (spotEnabled) {
        ZeroMemory(&spotLight, sizeof(spotLight));
        spotLight.Type = D3DLIGHT_SPOT;
        spotLight.Position = player.GetCameraPosition();
        spotLight.Direction = player.GetLookDirection();
        spotLight.Diffuse = D3DXCOLOR(1.0f, 0.95f, 0.8f, 1.0f);
        spotLight.Range = 30.0f;
        spotLight.Theta = D3DXToRadian(10.0f);
        spotLight.Phi = D3DXToRadian(25.0f);
        spotLight.Falloff = 1.0f;
        device->SetLight(1, &spotLight);
        device->LightEnable(1, TRUE);
    }
    else {
        device->LightEnable(1, FALSE);
    }

    // 3) (ewentualne aktualizacje prefabów, animacje)
}

void Scene::Render() {
    device->SetRenderState(D3DRS_LIGHTING, TRUE);
    // najpierw labirynt
    maze->Render();
}

bool Scene::CheckCollision(const D3DXVECTOR3& next, const D3DXVECTOR3&) const {
    return maze->HitTest(next) >= 0;
}
