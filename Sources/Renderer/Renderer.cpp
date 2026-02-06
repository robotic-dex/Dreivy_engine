#include "Renderer.h"
#include <dxgi.h>

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Init(HWND hwnd, uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;

    if (!CreateDeviceAndSwapChain(hwnd, width, height))
        return false;

    if (!CreateRenderTarget())
        return false;

    return true;
}

bool Renderer::CreateDeviceAndSwapChain(HWND hwnd, uint32_t width, uint32_t height) {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT flags = 0;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    return SUCCEEDED(
        D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            flags,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &sd,
            &m_swapChain,
            &m_device,
            nullptr,
            &m_context
        )
    );
}

bool Renderer::CreateRenderTarget() {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
        return false;

    return SUCCEEDED(
        m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_rtv)
    );
}

bool Renderer::CreateShaders()
{

   
    return true;
}

void Renderer::Resize(uint32_t width, uint32_t height) {
    if (!m_swapChain) return;

    m_context->OMSetRenderTargets(0, nullptr, nullptr);
    m_rtv.Reset();

    m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

    m_width = width;
    m_height = height;

    CreateRenderTarget();
}

void Renderer::BeginFrame(float r, float g, float b, float a) {
    float color[4]{ r, g, b, a };
    m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), nullptr);
    m_context->ClearRenderTargetView(m_rtv.Get(), color);
}

void Renderer::EndFrame() {
    m_swapChain->Present(1, 0);
}

void Renderer::Shutdown() {
    m_rtv.Reset();
    m_swapChain.Reset();
    m_context.Reset();
    m_device.Reset();
}

void Renderer::Render()
{
}
