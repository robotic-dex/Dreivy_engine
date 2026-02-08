#include "Core.h"
#include <windows.h>
#include <stdexcept>
#include "Math/Time.h"
Core::~Core() {
    Shutdown();
}


Core& Core::Init() {
    try {
        InitWindow();

        if (!InitSystem())
            throw std::runtime_error("InitSystem failed");

        SetupCallbacks();
        m_world = std::make_unique<World>();
        m_meshStorage = std::make_unique<MeshStorage>();
        m_renderQueue = std::make_unique<RenderQueue>();
        m_renderer->SetMeshStorage(m_meshStorage.get());
		// Call every function registered via addInitFunc ( only once)
        for (auto& f : m_initFuncs) {
            try { f(*this); }
            catch (...) { /* try to catch a error)) */ }
        }

    }
    catch (...) {
        MessageBoxW(nullptr, L"Core initialization failed", L"Error", MB_ICONERROR);
        m_running = false;
        return *this;
    }
   
    m_running = true;
    return *this;
}


Core& Core::Run() {
    while (m_running && m_window && m_window->ProcessMessages()) {
		Time::Update();
        Update();
        Draw();
    }
    return *this;
}

Core& Core::Shutdown() {
    if (m_renderer) {
        m_renderer->Shutdown();
        m_renderer.reset();
    }

    m_window.reset();
    m_running = false;

    return *this;
}


Core& Core::addFunc(const std::function<void(Core&)>& func) {
    m_funcs.push_back(func);
    return *this;
}

Core& Core::addInitFunc(const std::function<void(Core&)>& func) {
    m_initFuncs.push_back(func);
    return *this;
}


void Core::InitWindow() {
    WindowManager::Config cfg;
    cfg.title = L"Dreivy!";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.resizable = true;
    cfg.showCursor = true;
    cfg.captureMouse = false;

    m_window = WindowManager::Create(cfg);
}

bool Core::InitSystem() {
    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->Init(m_window->GetHWND(), m_window->GetWidth(), m_window->GetHeight()))
        return false;
    return true;
}

void Core::SetupCallbacks() {
    // Resize
    m_window->SetResizeCallback(
        [this](uint32_t w, uint32_t h) {
            if (w == 0 || h == 0) return;
            
            Resize_t.width = w;
            Resize_t.height = h;
            Resize_t.IsNeedResize = true;

        }
    );

    // Close
    m_window->SetCloseCallback(
        [this]() -> bool {
            m_running = false;
            return true;
        }
    );

    // Keyboard
    m_window->SetKeyCallback(
        [](uint32_t key, bool pressed) {
            if (key == VK_ESCAPE && pressed)
                PostQuitMessage(0);
        }
    );

    // Mouse move
    m_window->SetMouseMoveCallback(
        [](int x, int y, int dx, int dy) {
            
        }
    );
}


void Core::Update() {
    //if resize is needed we call renderer to resize.
	if (Resize_t.IsNeedResize && m_renderer) {
        m_renderer->Resize(Resize_t.width, Resize_t.height);
        Resize_t.IsNeedResize = false;
    }
	// Call every function registered via addFunc
    for (auto& f : m_funcs) {
        try { f(*this); }
        catch (...) {  }
    }
    // TODO  game logic
}


void Core::Draw() {
    if (!m_renderer) return;
    m_renderQueue->Clear();
    BuildRenderQueue(*m_world, *m_renderQueue);
   

    m_renderer->BeginFrame(0.1f, 0.1f, 0.15f, 1.0f);
    m_renderer->Draw(*m_renderQueue);
    m_renderer->EndFrame();
}
