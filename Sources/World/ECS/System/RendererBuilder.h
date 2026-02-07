// ECS/System/RenderQueueBuilder.h
#pragma once

#include "../World.h"
#include "Renderer/RenderQueue.h"

inline void BuildRenderQueue(World& world, RenderQueue& queue) {
    queue.Clear();


    const size_t count = world.transforms.size();

    for (Entity e = 0; e < count; ++e) {
        const Transform& t = world.GetTransform(e);
        const Mesh& m = world.GetMesh(e);

       
        if (m.meshId < 0)
            continue;

        queue.Submit(t, m.meshId);
    }
}
