#pragma once
#include <DirectXMath.h>
#include <World/ECS/Component/Transform.h>
using namespace DirectX;

inline XMMATRIX BuildWorldMatrix(const Transform& t) {
    XMMATRIX S = XMMatrixScaling(t.scale.x, t.scale.y, t.scale.z);
    XMMATRIX R = XMMatrixRotationRollPitchYaw(
        t.rotation.x,
        t.rotation.y,
        t.rotation.z
    );
    XMMATRIX T = XMMatrixTranslation(
        t.position.x,
        t.position.y,
        t.position.z
    );

    return S * R * T; // standard TRS
}
