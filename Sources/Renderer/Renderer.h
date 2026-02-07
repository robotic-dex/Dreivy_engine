#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

class RenderQueue;
struct Transform;

class Renderer {
public:
    Renderer() = default;
    ~Renderer();

    bool Init(HWND hwnd, uint32_t width, uint32_t height);
    void Resize(uint32_t width, uint32_t height);

    void BeginFrame(float r, float g, float b, float a);
    void Draw(const RenderQueue& queue);
    void EndFrame();
    void Shutdown();

private:
    struct Vertex {
        DirectX::XMFLOAT3 pos;
    };

    struct CB_Matrices {
        DirectX::XMFLOAT4X4 mvp;
    };

    bool CreateDeviceAndSwapChain(HWND hwnd, uint32_t width, uint32_t height);
    bool CreateRenderTarget();
    bool CreateDepthBuffer();
    bool CreateShaders();
    bool CreateCube();
    bool CreateConstantBuffer();
    bool CreateRasterizerState();

    void DrawMesh(int meshId, const Transform& t);

private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbMatrices;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterState;

    UINT m_indexCount = 0;
};
