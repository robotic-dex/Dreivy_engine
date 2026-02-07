#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "WindowManager/WindowManager.h"
#include "Renderer/Renderer.h"
struct RendererResizeEvent {
    uint32_t width;
    uint32_t height;
	bool IsNeedResize = false;
};
class Core {
public:
    Core() = default;
    ~Core();

    
    Core& Init();
    Core& Run();
    Core& Shutdown();

    
	Core& addFunc(const std::function<void(Core&)>& func);       // For Update(every frame)
	Core& addInitFunc(const std::function<void(Core&)>& func);   // For Init(only once after Init)

    // ===== Геттеры для внутренних систем =====
    WindowManager* getWindow() { return m_window.get(); }
    Renderer* getRenderer() { return m_renderer.get(); }

private:
    void InitWindow();
    bool InitSystem();
    void SetupCallbacks();

    void Update();
    void Draw();

private:
    std::unique_ptr<WindowManager> m_window;
    std::unique_ptr<Renderer>      m_renderer;

    bool m_running = false;
    RendererResizeEvent Resize_t;
	std::vector<std::function<void(Core&)>> m_funcs;      //  being called every frame in Run
	std::vector<std::function<void(Core&)>> m_initFuncs;  // called once at Init, after systems are initialized
};
