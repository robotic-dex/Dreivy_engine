// Renderer/MeshStorage.h
#pragma once
#include <vector>
#include <cassert>
#include "MeshData.h"
#include "MeshHandle.h"

class MeshStorage {
public:
    MeshHandle Add(const MeshData& data) {
        m_meshes.push_back(data);
        return static_cast<MeshHandle>(m_meshes.size()); // 1-based
    }

    const MeshData* Get(MeshHandle h) const {
        if (h == InvalidMesh) return nullptr;
        size_t idx = h - 1;
        assert(idx < m_meshes.size());
        return &m_meshes[idx];
    }

private:
    std::vector<MeshData> m_meshes;
};
