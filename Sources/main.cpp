#include <windows.h>
#include "Core.h"

#include <World/ECS/Entity/Entity.h>
#include <World/ECS/Component/Mesh.h>
#include <World/ECS/Component/Transform.h>

#include <Math/Time.h>

// renderer-side
#include <Renderer/MeshStorage.h>
#include <Renderer/StaticMeshes.h>

Entity g_cube = InvalidEntity;
Entity g_cube2 = InvalidEntity;

MeshHandle g_cubeMesh = InvalidMesh;

void setupScene(Core& core)
{
    World* world = core.getWorld();
    if (!world) return;

    // --- create test cube mesh (CPU) ---
    MeshData cube = CreateTestCube();
    MeshHandle h =
        core.getMeshStorage()->Add(CreateTestCube());


    // --------- first cube ----------
    g_cube = world->CreateEntity();

    world->AddComponent<Transform>(g_cube);
    world->AddComponent<Mesh>(g_cube);

    auto& t = world->GetComponent<Transform>(g_cube);
    t.position = { 0.0f, 0.0f, 0.0f };
    t.rotation = { 0.0f, 0.0f, 0.0f };
    t.scale = { 1.0f, 1.0f, 1.0f };

    world->GetComponent<Mesh>(g_cube).handle = g_cubeMesh;

    // --------- second cube ----------
    g_cube2 = world->CreateEntity();

    world->AddComponent<Transform>(g_cube2);
    world->AddComponent<Mesh>(g_cube2);

    auto& t2 = world->GetComponent<Transform>(g_cube2);
    t2.position = { 1.5f, 0.0f, 1.5f };
    t2.rotation = { 0.0f, 0.0f, 0.0f };
    t2.scale = { 1.0f, 1.0f, 1.0f };

    world->GetComponent<Mesh>(g_cube2).handle = g_cubeMesh;
}

void updateGame(Core& core)
{
    World* world = core.getWorld();
    if (!world) return;

    if (auto* t = world->TryGetComponent<Transform>(g_cube2)) {
        t->position.x = sinf(Time::time) * 3.0f;
        t->position.y = sinf(Time::time) * 3.0f;
    }
}

int WINAPI WinMain(
    HINSTANCE,
    HINSTANCE,
    LPSTR,
    int
) {
    auto core = std::make_unique<Core>();

    core->addInitFunc(setupScene)
        .addFunc(updateGame);

    core->Init();
    core->Run();
    core->Shutdown();

    return 0;
}
