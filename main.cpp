// main.cpp
// Główna pętla gry RetroFPS – inicjalizacja Direct3D9, tworzenie okna, obiektów,
// pętla renderowania (SimpleModel, TrailManager, UIOverlay), czyszczenie zasobów.

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9mesh.h>      // dla D3DXCreateBox, D3DXComputeNormals

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx9mesh.lib")

#include "SimpleModel.h"
#include "UIOverlay.h"
#include "TrailManager.h"
#include "Player.h"
#include "Scene.h"

//---------------------------------------------------------------------
// Stałe rozmiary okna
const UINT WINDOW_WIDTH = 800;
const UINT WINDOW_HEIGHT = 600;

// Globalne wskaźniki
HWND                  g_hWnd = nullptr;
IDirect3D9* g_pD3D = nullptr;
IDirect3DDevice9* g_pDevice = nullptr;

// Obiekty gry
SimpleModel           g_floorModel(nullptr);
UIOverlay* g_uiOverlay = nullptr;
TrailManager* g_trails = nullptr;
Player* g_player = nullptr;
Scene* g_scene = nullptr;

// Macierze kamery
D3DXMATRIX            g_matView;
D3DXMATRIX            g_matProj;

//---------------------------------------------------------------------
// Prototypy
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool InitWindow(HINSTANCE hInstance, int nCmdShow);
bool InitDirect3D();
void Cleanup();
void GameLoop();

//---------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    if (!InitWindow(hInstance, nCmdShow))
        return 0;
    if (!InitDirect3D()) {
        Cleanup();
        return 0;
    }

    // Inicjalizacja obiektów gry
    g_floorModel = SimpleModel::CreatePlane(g_pDevice, 20.0f, 20.0f);
    g_floorModel.SetPosition(0.0f, -1.0f, 0.0f);
    g_floorModel.SetTexture(nullptr);  // załaduj własną teksturą lub zostaw brak

    g_uiOverlay = new UIOverlay(g_pDevice);
    g_trails = new TrailManager(g_pDevice);
    g_player = new Player();       // implementacja Playera w Player.cpp/h
    g_scene = new Scene();        // scena może zawierać listę modeli, kamery, itp.

    // Ustawienia kamery
    D3DXVECTOR3 eye(0.0f, 5.0f, -10.0f);
    D3DXVECTOR3 at(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMatrixLookAtLH(&g_matView, &eye, &at, &up);
    D3DXMatrixPerspectiveFovLH(
        &g_matProj,
        D3DX_PI / 4.0f,
        float(WINDOW_WIDTH) / WINDOW_HEIGHT,
        0.1f, 100.0f
    );

    // Główna pętla
    GameLoop();

    // Sprzątanie
    Cleanup();
    return 0;
}

//---------------------------------------------------------------------
bool InitWindow(HINSTANCE hInstance, int nCmdShow) {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"RetroFPSWindow";

    if (!RegisterClassEx(&wc))
        return false;

    g_hWnd = CreateWindow(
        wc.lpszClassName,
        L"RetroFPS - Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        nullptr, nullptr,
        hInstance, nullptr
    );
    if (!g_hWnd)
        return false;

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);
    return true;
}

//---------------------------------------------------------------------
bool InitDirect3D() {
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_pD3D) return false;

    D3DPRESENT_PARAMETERS pp = {};
    pp.Windowed = TRUE;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.BackBufferFormat = D3DFMT_X8R8G8B8;
    pp.BackBufferWidth = WINDOW_WIDTH;
    pp.BackBufferHeight = WINDOW_HEIGHT;
    pp.EnableAutoDepthStencil = TRUE;
    pp.AutoDepthStencilFormat = D3DFMT_D24S8;

    if (FAILED(g_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        g_hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &pp,
        &g_pDevice
    ))) {
        return false;
    }

    // Ustawienia globalne renderingu
    g_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    return true;
}

//---------------------------------------------------------------------
void GameLoop() {
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // --- Aktualizacja stanu gry ---
        g_player->Update();
        g_scene->Update();
        g_trails->AddPoint(g_player->GetPosition());

        // --- Renderowanie ---
        g_pDevice->Clear(
            0, nullptr,
            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
            D3DCOLOR_XRGB(30, 30, 40),
            1.0f, 0
        );
        if (SUCCEEDED(g_pDevice->BeginScene())) {
            g_pDevice->SetTransform(D3DTS_VIEW, &g_matView);
            g_pDevice->SetTransform(D3DTS_PROJECTION, &g_matProj);

            g_floorModel.Render(g_pDevice);
            for (auto& m : g_scene->GetModels()) {
                m.Render(g_pDevice);
            }

            g_trails->Render(g_matView, g_matProj);
            g_uiOverlay->Render(*g_player, *g_scene);

            g_pDevice->EndScene();
        }
        g_pDevice->Present(nullptr, nullptr, nullptr, nullptr);
    }
}

//---------------------------------------------------------------------
void Cleanup() {
    delete g_uiOverlay;
    delete g_trails;
    delete g_player;
    delete g_scene;

    if (g_pDevice) {
        g_pDevice->Release();
        g_pDevice = nullptr;
    }
    if (g_pD3D) {
        g_pD3D->Release();
        g_pD3D = nullptr;
    }
}

//---------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
