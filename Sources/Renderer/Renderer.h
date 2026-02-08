#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string_view>
#include "MeshStorage.h"
#include <unordered_map>
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
    void DrawMesh(MeshHandle mesh, DirectX::XMMATRIX world);
    void EndFrame();
    void Shutdown();
    void SetMeshStorage(MeshStorage* storage) {
        m_meshStorage = storage;
    }
private:
    struct Vertex {
        DirectX::XMFLOAT3 pos;
    };

    struct alignas(16) CB_Matrices {
        DirectX::XMFLOAT4X4 mvp;
    };

    bool CreateDeviceAndSwapChain(HWND hwnd, uint32_t width, uint32_t height);
    bool CreateRenderTarget();
    bool CreateDepthBuffer();
    bool CreateShaders();
    bool CreateCube();
    bool CreateConstantBuffer();
    bool CreateRasterizerState();

    

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
    struct GpuMesh {
        Microsoft::WRL::ComPtr<ID3D11Buffer> vb;
        Microsoft::WRL::ComPtr<ID3D11Buffer> ib;
        UINT indexCount = 0;
    };

    std::unordered_map<MeshHandle, GpuMesh> m_gpuMeshes;
    MeshStorage* m_meshStorage = nullptr; // injected

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterState;

    UINT m_indexCount = 0;
};
