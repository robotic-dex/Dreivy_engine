#include <windows.h>
#include "Core.h"

// Test if fucntions is really called once?
void setupScene(Core& core) {
    World* world = core.getWorld();
    if (!world) return;

    Entity e = world->CreateEntity();

    Transform& t = world->GetTransform(e);
    t.position = { 0.0f, 0.0f, 0.0f };
    t.rotation = { 0.0f, 0.0f, 0.0f };
    t.scale = { 1.0f, 1.0f, 1.0f };

    Mesh& m = world->GetMesh(e);
    m.meshId = 0; 
}



void updateGame(Core& core) {
	// call other functions or write some gamelogic here, this will be called every frame in Run
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    LPSTR,
    int
) {
    auto core = std::make_unique<Core>();

    // Регистрируем функции
	core->addInitFunc(&setupScene)   // will be called once at Init, after systems are initialized
		.addFunc(&updateGame);      // will be called every frame in Run

    
    core->Init()
        .Run()
        .Shutdown();

    return 0;
}
