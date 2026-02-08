#pragma once
#include <vector>
#include <string_view>
#include <DirectXMath.h>
#include "Renderer/MeshHandle.h"
using namespace DirectX;
using Entity = uint32_t;


struct RenderItem {
    DirectX::XMFLOAT4X4 world;
    MeshHandle mesh;
    Entity id;
};



class RenderQueue {
public:
    void Submit(const XMFLOAT4X4& world, MeshHandle mesh, Entity id) {
        items.push_back({ world, mesh, id });
    }




    const std::vector<RenderItem>& GetItems() const {
        return items;
    }

    void Clear() {
        items.clear();
    }

private:
    std::vector<RenderItem> items;
};
