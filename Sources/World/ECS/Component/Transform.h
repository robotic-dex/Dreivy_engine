#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct Transform {
    XMFLOAT3 position{ 0,0,0 };
    XMFLOAT3 rotation{ 0,0,0 };
    XMFLOAT3 scale{ 1,1,1 };
};
