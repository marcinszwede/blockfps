#include "SimpleModel.h"
#include <algorithm>
#include <cassert>

SimpleModel::SimpleModel(IDirect3DDevice9* dev)
    : mesh(nullptr),
    device(dev),
    texture(nullptr),
    position(0, 0, 0),
    scale(1, 1, 1)
{
    assert(device);
    D3DXMatrixIdentity(&rotation);
    D3DXMatrixIdentity(&world);

    // Domyœlnie twórz pude³ko 1x1x1, ¿eby mesh != nullptr
    if (FAILED(D3DXCreateBox(device, 1.0f, 1.0f, 1.0f, &mesh, nullptr))) {
        mesh = nullptr;
    }

    ZeroMemory(&material, sizeof(material));
    material.Diffuse = material.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);

    UpdateWorld();
    ComputeBounds();
}

SimpleModel::~SimpleModel() {
    if (mesh) {
        mesh->Release();
        mesh = nullptr;
    }
}

SimpleModel SimpleModel::CreatePlane(IDirect3DDevice9* dev, float width, float depth) {
    SimpleModel m(dev);

    // Zniszcz box z konstruktora
    if (m.mesh) { m.mesh->Release(); m.mesh = nullptr; }

    // W³asnorêczna budowa prostego plane
    constexpr UINT numVerts = 4;
    constexpr UINT numFaces = 2;
    HRESULT hr = D3DXCreateMeshFVF(
        numFaces,
        numVerts,
        D3DXMESH_MANAGED,
        Vertex::FVF,
        dev,
        &m.mesh
    );
    if (FAILED(hr) || !m.mesh) {
        return m;  // mesh == nullptr
    }

    // Wype³nij wierzcho³ki
    Vertex* v = nullptr;
    if (SUCCEEDED(m.mesh->LockVertexBuffer(0, reinterpret_cast<void**>(&v)))) {
        float hw = width * 0.5f, hd = depth * 0.5f;
        // po kolei: (-hw,0,-hd), (+hw,0,-hd), (-hw,0,+hd), (+hw,0,+hd)
        v[0] = { {-hw, 0, -hd}, {0,1,0}, {0,0} };
        v[1] = { {+hw, 0, -hd}, {0,1,0}, {1,0} };
        v[2] = { {-hw, 0, +hd}, {0,1,0}, {0,1} };
        v[3] = { {+hw, 0, +hd}, {0,1,0}, {1,1} };
        m.mesh->UnlockVertexBuffer();
    }

    // Wype³nij indeksy
    DWORD* idx = nullptr;
    if (SUCCEEDED(m.mesh->LockIndexBuffer(0, reinterpret_cast<void**>(&idx)))) {
        // dwa trójk¹ty: (0,1,2) oraz (2,1,3)
        idx[0] = 0; idx[1] = 1; idx[2] = 2;
        idx[3] = 2; idx[4] = 1; idx[5] = 3;
        m.mesh->UnlockIndexBuffer();
    }

    // Liczymy normalne:
    D3DXComputeNormals(m.mesh, nullptr);

    D3DXMatrixIdentity(&m.rotation);
    m.UpdateWorld();
    m.ComputeBounds();
    return m;
}

void SimpleModel::SetPosition(float x, float y, float z) {
    position = { x,y,z };
    UpdateWorld();
}

void SimpleModel::SetRotation(float pitch, float yaw, float roll) {
    D3DXMATRIX Rx, Ry, Rz;
    D3DXMatrixRotationX(&Rx, pitch);
    D3DXMatrixRotationY(&Ry, yaw);
    D3DXMatrixRotationZ(&Rz, roll);
    rotation = Rx * Ry * Rz;
    UpdateWorld();
}

void SimpleModel::SetScale(float s) {
    scale = { s,s,s };
    UpdateWorld();
}

void SimpleModel::SetTexture(IDirect3DTexture9* tex) {
    texture = tex;
}

void SimpleModel::SetAlpha(float a) {
    material.Diffuse.a = material.Ambient.a = a;
}

void SimpleModel::UpdateWorld() {
    D3DXMATRIX S, T;
    D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
    D3DXMatrixTranslation(&T, position.x, position.y, position.z);
    world = S * rotation * T;
}

void SimpleModel::ComputeBounds() {
    if (!mesh) return;

    Vertex* verts = nullptr;
    if (FAILED(mesh->LockVertexBuffer(0, reinterpret_cast<void**>(&verts)))) return;

    UINT count = mesh->GetNumVertices();
    if (count) {
        minB = maxB = verts[0].pos;
        for (UINT i = 1; i < count; ++i) {
            const auto& p = verts[i].pos;
            minB.x = std::min(minB.x, p.x);
            minB.y = std::min(minB.y, p.y);
            minB.z = std::min(minB.z, p.z);
            maxB.x = std::max(maxB.x, p.x);
            maxB.y = std::max(maxB.y, p.y);
            maxB.z = std::max(maxB.z, p.z);
        }
    }
    mesh->UnlockVertexBuffer();
}

void SimpleModel::Render(IDirect3DDevice9* dev) const {
    if (!mesh) return;
    dev->SetTransform(D3DTS_WORLD, const_cast<D3DXMATRIX*>(&world));
    dev->SetMaterial(const_cast<D3DMATERIAL9*>(&material));
    dev->SetTexture(0, texture);
    mesh->DrawSubset(0);
}

bool SimpleModel::Intersects(const D3DXVECTOR3& p) const {
    return (p.x >= minB.x && p.x <= maxB.x)
        && (p.y >= minB.y && p.y <= maxB.y)
        && (p.z >= minB.z && p.z <= maxB.z);
}
