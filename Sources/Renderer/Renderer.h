#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
//TODO : ECS and struct Renderable objects

class Renderer {
public:
    Renderer() = default;
    ~Renderer();

    bool Init(HWND hwnd, uint32_t width, uint32_t height);
    void Resize(uint32_t width, uint32_t height);

    void BeginFrame(float r, float g, float b, float a);
    void EndFrame();
    void Shutdown();

    void Render();

private:
    bool CreateDeviceAndSwapChain(HWND hwnd, uint32_t width, uint32_t height);
    bool CreateRenderTarget();
    bool CreateShaders(); 

private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;

    // Простые шейдеры
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};
