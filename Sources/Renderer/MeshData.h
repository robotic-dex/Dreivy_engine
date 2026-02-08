#pragma once
#include <vector>
#include <DirectXMath.h>

struct MeshData {
    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<uint32_t> indices;
};
