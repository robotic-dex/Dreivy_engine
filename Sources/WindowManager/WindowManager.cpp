#include "WindowManager.h"
#include <vector>
#include <cstring>
#include <stdexcept>
#include <windowsx.h>

struct WindowData {
    HWND hwnd = nullptr;
    HINSTANCE hInstance = nullptr;

    WindowManager::Config config;

    uint32_t clientWidth = 0;
    uint32_t clientHeight = 0;

    bool minimized = false;
    bool maximized = false;
    bool active = true;
    bool closed = false;
    bool mouseCaptured = false;
    bool initialized = false;

    static constexpr uint32_t KEY_COUNT = 256;
    static constexpr uint32_t MOUSE_BUTTON_COUNT = 3;

    bool keys[KEY_COUNT]{};
    bool prevKeys[KEY_COUNT]{};
    bool mouseButtons[MOUSE_BUTTON_COUNT]{};
    bool prevMouseButtons[MOUSE_BUTTON_COUNT]{};

    int32_t mouseX = 0, mouseY = 0;
    int32_t prevMouseX = 0, prevMouseY = 0;

    ResizeCallback onResize;
    CloseCallback onClose;
    InputCallback onInput;
    MouseMoveCallback onMouseMove;
    MouseButtonCallback onMouseButton;
    KeyCallback onKey;
};


std::unique_ptr<WindowManager> WindowManager::Create(const Config& config) {
    auto ptr = std::unique_ptr<WindowManager>(new WindowManager(config));
    if (!ptr->Initialize())
        throw std::runtime_error("WindowManager init failed");
    return ptr;
}


WindowManager::WindowManager(const Config& config)
    : m_data(std::make_unique<WindowData>()) {
    m_data->config = config;
}
void WindowManager::CalculateClientSize()
{
    RECT rect{};
    GetClientRect(m_data->hwnd, &rect);

    m_data->clientWidth = rect.right - rect.left;
    m_data->clientHeight = rect.bottom - rect.top;
}

WindowManager::~WindowManager() {
    Shutdown();
}


bool WindowManager::Initialize() {
    if (m_data->initialized) return true;

    if (!CreateWindowClass()) return false;
    if (!CreateWindowInstance()) return false;

    CalculateClientSize();
    ShowWindow(m_data->hwnd, SW_SHOW);

    if (!m_data->config.showCursor)
        ::ShowCursor(FALSE);

    if (m_data->config.captureMouse)
        SetMouseCapture(true);

    m_data->initialized = true;
    return true;
}

void WindowManager::Shutdown() {
    if (!m_data->initialized) return;
    DestroyWindow(m_data->hwnd);
    UnregisterClassW(L"GameEngineWindow", m_data->hInstance);
    m_data->closed = true;
    m_data->initialized = false;
}


bool WindowManager::CreateWindowClass() {
    m_data->hInstance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = StaticWndProc;
    wc.hInstance = m_data->hInstance;
    wc.lpszClassName = L"GameEngineWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = m_data->config.icon ? m_data->config.icon : LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = m_data->config.iconSmall ? m_data->config.iconSmall : wc.hIcon;

    return RegisterClassExW(&wc) != 0;
}

bool WindowManager::CreateWindowInstance() {
    DWORD style = WS_OVERLAPPEDWINDOW;
    if (!m_data->config.resizable)
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

    RECT r{ 0, 0, (LONG)m_data->config.width, (LONG)m_data->config.height };
    AdjustWindowRect(&r, style, FALSE);

    m_data->hwnd = CreateWindowExW(
        0,
        L"GameEngineWindow",
        m_data->config.title.c_str(),
        style,
        m_data->config.posX,
        m_data->config.posY,
        r.right - r.left,
        r.bottom - r.top,
        nullptr,
        nullptr,
        m_data->hInstance,
        this
    );

    return m_data->hwnd != nullptr;
}


LRESULT CALLBACK WindowManager::StaticWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    WindowManager* self = nullptr;

    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCT*>(lp);
        self = static_cast<WindowManager*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
    }
    else {
        self = (WindowManager*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    return self ? self->HandleMessage(hwnd, msg, wp, lp)
        : DefWindowProcW(hwnd, msg, wp, lp);
}

LRESULT WindowManager::HandleMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (m_data->onInput)
        m_data->onInput(msg, wp, lp);

    switch (msg) {
    case WM_CLOSE:
        if (!m_data->onClose || m_data->onClose())
            DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        m_data->closed = true;
        return 0;

    case WM_ACTIVATE:
        m_data->active = (LOWORD(wp) != WA_INACTIVE);
        return 0;

    case WM_SIZE:
        m_data->clientWidth = LOWORD(lp);
        m_data->clientHeight = HIWORD(lp);
        if (m_data->onResize)
            m_data->onResize(m_data->clientWidth, m_data->clientHeight);
        return 0;

    case WM_KEYDOWN: ProcessKey((uint32_t)wp, true); break;
    case WM_KEYUP:   ProcessKey((uint32_t)wp, false); break;

    case WM_LBUTTONDOWN: ProcessMouseButton(0, true); break;
    case WM_LBUTTONUP:   ProcessMouseButton(0, false); break;
    case WM_RBUTTONDOWN: ProcessMouseButton(1, true); break;
    case WM_RBUTTONUP:   ProcessMouseButton(1, false); break;
    case WM_MBUTTONDOWN: ProcessMouseButton(2, true); break;
    case WM_MBUTTONUP:   ProcessMouseButton(2, false); break;

    case WM_MOUSEMOVE:
        ProcessMouseMove(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        break;
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}


void WindowManager::ProcessKey(uint32_t key, bool pressed) {
    if (key >= WindowData::KEY_COUNT) return;
    m_data->keys[key] = pressed;
    if (m_data->onKey) m_data->onKey(key, pressed);
}

void WindowManager::ProcessMouseButton(uint32_t b, bool pressed) {
    if (b >= WindowData::MOUSE_BUTTON_COUNT) return;
    m_data->mouseButtons[b] = pressed;
    if (m_data->onMouseButton) m_data->onMouseButton(b, pressed);
}

void WindowManager::ProcessMouseMove(int32_t x, int32_t y) {
    m_data->prevMouseX = m_data->mouseX;
    m_data->prevMouseY = m_data->mouseY;
    m_data->mouseX = x;
    m_data->mouseY = y;

    if (m_data->onMouseMove)
        m_data->onMouseMove(x, y, x - m_data->prevMouseX, y - m_data->prevMouseY);
}


bool WindowManager::ProcessMessages() {
    memcpy(m_data->prevKeys, m_data->keys, sizeof(m_data->keys));
    memcpy(m_data->prevMouseButtons, m_data->mouseButtons, sizeof(m_data->mouseButtons));

    MSG msg{};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            return false;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return true;
}


HWND WindowManager::GetHWND() const { return m_data->hwnd; }
HINSTANCE WindowManager::GetHInstance() const { return m_data->hInstance; }
uint32_t WindowManager::GetWidth() const { return m_data->clientWidth; }
uint32_t WindowManager::GetHeight() const { return m_data->clientHeight; }
float WindowManager::GetAspectRatio() const {
    return m_data->clientHeight ? float(m_data->clientWidth) / m_data->clientHeight : 0.f;
}
bool WindowManager::IsClosed() const { return m_data->closed; }


void WindowManager::SetTitle(const std::wstring& t) {
    m_data->config.title = t;
    SetWindowTextW(m_data->hwnd, t.c_str());
}

void WindowManager::SetMouseCapture(bool c) {
    m_data->mouseCaptured = c;
    if (!c) { ClipCursor(nullptr); return; }

    RECT r; GetClientRect(m_data->hwnd, &r);
    MapWindowPoints(m_data->hwnd, nullptr, (POINT*)&r, 2);
    ClipCursor(&r);
}

void WindowManager::SetResizeCallback(ResizeCallback cb) { m_data->onResize = cb; }
void WindowManager::SetCloseCallback(CloseCallback cb) { m_data->onClose = cb; }
void WindowManager::SetInputCallback(InputCallback cb) { m_data->onInput = cb; }
void WindowManager::SetMouseMoveCallback(MouseMoveCallback cb) { m_data->onMouseMove = cb; }
void WindowManager::SetMouseButtonCallback(MouseButtonCallback cb) { m_data->onMouseButton = cb; }
void WindowManager::SetKeyCallback(KeyCallback cb) { m_data->onKey = cb; }


bool WindowManager::IsKeyDown(uint32_t k) const { return m_data->keys[k]; }
bool WindowManager::IsKeyPressed(uint32_t k) const {
    return m_data->keys[k] && !m_data->prevKeys[k];
}
bool WindowManager::IsKeyReleased(uint32_t k) const {
    return !m_data->keys[k] && m_data->prevKeys[k];
}
bool WindowManager::IsMouseButtonDown(uint32_t b) const {
    return m_data->mouseButtons[b];
}


std::wstring WindowManager::StringToWide(const std::string& s) {
    int sz = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring r(sz, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, r.data(), sz);
    return r;
}

std::string WindowManager::WideToString(const std::wstring& s) {
    int sz = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string r(sz, 0);
    WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, r.data(), sz, nullptr, nullptr);
    return r;
}
