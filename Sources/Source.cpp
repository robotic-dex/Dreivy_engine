#include <windows.h>
#include "Core.h"

// Test if fucntions is really called once?
void setupScene(Core& core) {
    if (auto renderer = core.getRenderer()) {
        MessageBoxA(nullptr, "Called once at Init?", "Info", MB_OK);
    }
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
