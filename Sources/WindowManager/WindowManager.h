#pragma once
#include <windows.h>
#include <memory>
#include <functional>
#include <string>
#include <cstdint>


using ResizeCallback = std::function<void(uint32_t, uint32_t)>;
using CloseCallback = std::function<bool()>;
using InputCallback = std::function<void(UINT, WPARAM, LPARAM)>;
using MouseMoveCallback = std::function<void(int32_t, int32_t, int32_t, int32_t)>;
using MouseButtonCallback = std::function<void(int32_t, bool)>;
using KeyCallback = std::function<void(uint32_t, bool)>;

struct WindowData;

class WindowManager {
public:
    struct Config {
        std::wstring title = L"Game Engine";
        uint32_t width = 1280;
        uint32_t height = 720;
        int32_t posX = CW_USEDEFAULT;
        int32_t posY = CW_USEDEFAULT;
        bool resizable = true;
        bool showCursor = true;
        bool captureMouse = false;
        HICON icon = nullptr;
        HICON iconSmall = nullptr;
    };

public:
    static std::unique_ptr<WindowManager> Create(const Config& config = {});
    ~WindowManager();

    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    bool Initialize();
    void Shutdown();
    bool ProcessMessages();

   
    HWND GetHWND() const;
    HINSTANCE GetHInstance() const;

   
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    float GetAspectRatio() const;
    bool IsMinimized() const;
    bool IsMaximized() const;
    bool IsActive() const;
    bool IsClosed() const;

   
    void SetTitle(const std::wstring& title);
    void SetSize(uint32_t width, uint32_t height);
    void SetPosition(int32_t x, int32_t y);
    void CenterOnScreen();
    void Minimize();
    void Maximize();
    void Restore();
    void Show();
    void Hide();
    void SetResizable(bool resizable);
    void SetAlwaysOnTop(bool onTop);
    void SetCursorVisibility(bool visible);
    void SetMouseCapture(bool capture);

    
    void SetResizeCallback(ResizeCallback cb);
    void SetCloseCallback(CloseCallback cb);
    void SetInputCallback(InputCallback cb);
    void SetMouseMoveCallback(MouseMoveCallback cb);
    void SetMouseButtonCallback(MouseButtonCallback cb);
    void SetKeyCallback(KeyCallback cb);

   
    bool IsKeyDown(uint32_t key) const;
    bool IsKeyPressed(uint32_t key) const;
    bool IsKeyReleased(uint32_t key) const;
    bool IsMouseButtonDown(uint32_t button) const;
    void GetMousePosition(int32_t& x, int32_t& y) const;
    void SetMousePosition(int32_t x, int32_t y);


    static std::wstring StringToWide(const std::string& str);
    static std::string  WideToString(const std::wstring& wstr);

private:
    explicit WindowManager(const Config& config);

    bool CreateWindowClass();
    bool CreateWindowInstance();
    void UpdateWindowStyle();
    void CalculateClientSize();

    static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT HandleMessage(HWND, UINT, WPARAM, LPARAM);

    void ProcessKey(uint32_t key, bool pressed);
    void ProcessMouseButton(uint32_t button, bool pressed);
    void ProcessMouseMove(int32_t x, int32_t y);

private:
    std::unique_ptr<WindowData> m_data;
};
