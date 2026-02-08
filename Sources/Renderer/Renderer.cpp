#include "Renderer.h"
#include "RenderQueue.h"
#include "world/ecs/component/Transform.h"

#include <dxgi.h>
#include <d3dcompiler.h>
#include <string>
using namespace DirectX;

Renderer::~Renderer() { Shutdown(); }

bool Renderer::Init(HWND hwnd, uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;

    if (!CreateDeviceAndSwapChain(hwnd, width, height)) goto fail;
    if (!CreateRenderTarget()) goto fail;
    if (!CreateDepthBuffer()) goto fail;
    if (!CreateShaders()) goto fail;
    if (!CreateConstantBuffer()) goto fail;
    if (!CreateRasterizerState()) goto fail;
    
    return true;
fail:
	MessageBoxA(hwnd, "Failed to initialize renderer.", "Initialization Error", MB_OK | MB_ICONERROR);
    return false;
}
GpuMesh& Renderer::GetOrCreateGpuMesh(MeshHandle handle)
{
    auto it = m_gpuMeshes.find(handle);
    if (it != m_gpuMeshes.end())
        return it->second;

    const MeshData* cpu = m_meshStorage->Get(handle);
    assert(cpu && "Invalid MeshHandle");

    GpuMesh gpu = CreateGpuMesh(*cpu);
    return m_gpuMeshes.emplace(handle, std::move(gpu)).first->second;
}

GpuMesh Renderer::CreateGpuMesh(const MeshData& cpu)
{
    GpuMesh mesh;
    mesh.indexCount = static_cast<UINT>(cpu.indices.size());

    // --- Vertex buffer ---
    std::vector<Vertex> vertices;
    vertices.reserve(cpu.positions.size());

    for (const auto& p : cpu.positions) {
        vertices.push_back({ p });
    }

    D3D11_BUFFER_DESC vbd{};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = UINT(vertices.size() * sizeof(Vertex));
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vinit{};
    vinit.pSysMem = vertices.data();

    m_device->CreateBuffer(&vbd, &vinit, &mesh.vb);

    
    D3D11_BUFFER_DESC ibd{};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = UINT(cpu.indices.size() * sizeof(uint32_t));
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA iinit{};
    iinit.pSysMem = cpu.indices.data();

    m_device->CreateBuffer(&ibd, &iinit, &mesh.ib);

    return mesh;
}

bool Renderer::CreateDeviceAndSwapChain(HWND hwnd, uint32_t w, uint32_t h) {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = w;
    sd.BufferDesc.Height = h;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    UINT flags = 0;
#if _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    return SUCCEEDED(D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
        nullptr, 0, D3D11_SDK_VERSION,
        &sd, &m_swapChain, &m_device, nullptr, &m_context));
}

bool Renderer::CreateRenderTarget() {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> bb;
    if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&bb)))) return false;
    return SUCCEEDED(m_device->CreateRenderTargetView(bb.Get(), nullptr, &m_rtv));
}

bool Renderer::CreateDepthBuffer() {
    D3D11_TEXTURE2D_DESC d{};
    d.Width = m_width;
    d.Height = m_height;
    d.MipLevels = 1;
    d.ArraySize = 1;
    d.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d.SampleDesc.Count = 1;
    d.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    if (FAILED(m_device->CreateTexture2D(&d, nullptr, &tex))) return false;
    return SUCCEEDED(m_device->CreateDepthStencilView(tex.Get(), nullptr, &m_dsv));
}

bool Renderer::CreateShaders()
{
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> error;

    
    HRESULT hr = D3DCompileFromFile(
        L"Simple.hlsl",
        nullptr,
        nullptr,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS,
        0,
        &vsBlob,
        &error
    );

    if (FAILED(hr))
    {
        if (error)
            MessageBoxA(nullptr, (char*)error->GetBufferPointer(),
                "Vertex Shader Compile Error", MB_OK | MB_ICONERROR);
        return false;
    }

    if (FAILED(m_device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        &m_vs)))
        return false;

    
    error.Reset();

    hr = D3DCompileFromFile(
        L"Simple.hlsl",
        nullptr,
        nullptr,
        "PSMain",
        "ps_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS,
        0,
        &psBlob,
        &error
    );

    if (FAILED(hr))
    {
        if (error)
            MessageBoxA(nullptr, (char*)error->GetBufferPointer(),
                "Pixel Shader Compile Error", MB_OK | MB_ICONERROR);
        return false;
    }

    if (FAILED(m_device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        &m_ps)))
        return false;

   
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
          D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    if (FAILED(m_device->CreateInputLayout(
        layout,
        1,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &m_inputLayout)))
        return false;

    return true;
}


bool Renderer::CreateCube() {
    Vertex v[] = {
        {{-0.5f,-0.5f,-0.5f}},{{-0.5f,0.5f,-0.5f}},{{0.5f,0.5f,-0.5f}},{{0.5f,-0.5f,-0.5f}},
        {{-0.5f,-0.5f,0.5f}}, {{-0.5f,0.5f,0.5f}}, {{0.5f,0.5f,0.5f}}, {{0.5f,-0.5f,0.5f}}
    };

    uint16_t i[] = {
        0,1,2,0,2,3, 4,6,5,4,7,6,
        4,5,1,4,1,0, 3,2,6,3,6,7,
        1,5,6,1,6,2, 4,0,3,4,3,7
    };

    m_indexCount = _countof(i);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(v);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init{ v };
    m_device->CreateBuffer(&bd, &init, &m_vertexBuffer);

    bd.ByteWidth = sizeof(i);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    init.pSysMem = i;

    return SUCCEEDED(m_device->CreateBuffer(&bd, &init, &m_indexBuffer));
}

bool Renderer::CreateConstantBuffer() {
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_Matrices);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    return SUCCEEDED(m_device->CreateBuffer(&bd, nullptr, &m_cbMatrices));
}

bool Renderer::CreateRasterizerState() {
    D3D11_RASTERIZER_DESC rd{};
  //  rd.FillMode = D3D11_FILL_SOLID;
  //  rd.CullMode = D3D11_CULL_NONE;
    rd.FillMode = D3D11_FILL_WIREFRAME;
    rd.CullMode = D3D11_CULL_NONE;

    rd.DepthClipEnable = TRUE;
    return SUCCEEDED(m_device->CreateRasterizerState(&rd, &m_rasterState));
}

void Renderer::BeginFrame(float r, float g, float b, float a) {
    float c[4]{ r,g,b,a };
    m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());
    m_context->ClearRenderTargetView(m_rtv.Get(), c);
    m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH, 1, 0);
    m_context->RSSetState(m_rasterState.Get());

    D3D11_VIEWPORT vp{ 0,0,(float)m_width,(float)m_height,0,1 };
    m_context->RSSetViewports(1, &vp);
}

void Renderer::Draw(const RenderQueue& q)
{
    for (const auto& item : q.GetItems()) {
        DirectX::XMMATRIX world =
            DirectX::XMLoadFloat4x4(&item.world);

        DrawMesh(item.mesh, world);
    }
}
void Renderer::DrawMesh(MeshHandle mesh, DirectX::XMMATRIX world)
{
    using namespace DirectX;

    GpuMesh& gm = GetOrCreateGpuMesh(mesh);

    XMMATRIX view = XMMatrixLookAtLH(
        XMVectorSet(0, 0, -5, 1),
        XMVectorSet(0, 0, 0, 1),
        XMVectorSet(0, 1, 0, 0)
    );

    XMMATRIX proj = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,
        float(m_width) / m_height,
        0.1f,
        1000.0f
    );

    CB_Matrices cb;
    XMStoreFloat4x4(&cb.mvp, world * view * proj);

    m_context->UpdateSubresource(
        m_cbMatrices.Get(), 0, nullptr, &cb, 0, 0
    );

    m_context->VSSetConstantBuffers(0, 1, m_cbMatrices.GetAddressOf());

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    m_context->IASetVertexBuffers(
        0, 1, gm.vb.GetAddressOf(), &stride, &offset
    );

    m_context->IASetIndexBuffer(
        gm.ib.Get(), DXGI_FORMAT_R32_UINT, 0
    );

    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_context->IASetInputLayout(m_inputLayout.Get());
    m_context->VSSetShader(m_vs.Get(), nullptr, 0);
    m_context->PSSetShader(m_ps.Get(), nullptr, 0);

    m_context->DrawIndexed(gm.indexCount, 0, 0);
}



void Renderer::EndFrame() { m_swapChain->Present(0, 0); }

void Renderer::Resize(uint32_t w, uint32_t h) {
    if (!m_swapChain) return;
    m_context->OMSetRenderTargets(0, nullptr, nullptr);
    m_rtv.Reset(); m_dsv.Reset();
    m_swapChain->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);
    m_width = w; m_height = h;
    CreateRenderTarget();
    CreateDepthBuffer();
}

void Renderer::Shutdown() {
    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
    m_cbMatrices.Reset();
    m_inputLayout.Reset();
    m_vs.Reset();
    m_ps.Reset();
    m_rasterState.Reset();
    m_dsv.Reset();
    m_rtv.Reset();
    m_swapChain.Reset();
    m_context.Reset();
    m_device.Reset();
}
