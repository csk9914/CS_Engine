#include "DX11Renderer.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib") // 나중에 쉐이더 쓸 때 필요함
#pragma comment(lib, "dxgi.lib")

DX11Renderer::~DX11Renderer()
{
    // ComPtr이 자동으로 Release() 호출 — 수동 정리 불필요
    if (m_context) m_context->ClearState();
}

std::unique_ptr<DX11Renderer> DX11Renderer::Create(HWND hWnd, int width, int height)
{
    // private 생성자 우회 (make_unique 불가)
    std::unique_ptr<DX11Renderer> renderer(new DX11Renderer());
    if (!renderer->Init(hWnd, width, height)) return nullptr;

    return renderer;
}

bool DX11Renderer::Init(HWND hWnd, int width, int height)
{
    m_width = width;
    m_height = height;

    // 스왑 체인 + 디바이스 + 컨텍스트 생성
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = (UINT)width;
    scd.BufferDesc.Height = (UINT)height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,                    // 기본 어댑터
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,                          // 디버그 플래그 원하면 D3D11_CREATE_DEVICE_DEBUG
        nullptr, 0,                 // 피처 레벨 배열 (nullptr = 자동)
        D3D11_SDK_VERSION,
        &scd,
        &m_swapChain,
        &m_device,
        nullptr,
        &m_context
    );
    if (FAILED(hr)) return false;

    // back buffer 생성
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr)) return false;
    
    // Render Target View 생성
    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_rtv);
    if (FAILED(hr)) return false;

    // depth stencil buffer 생성
    if (!CreateDepthBuffer(width, height)) return false;

    // Output Merger에 바인딩
    m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());

    // view port 설정
    m_viewport = { 0.f, 0.f, (float)width, (float)height, 0.f, 1.f };
    m_context->RSSetViewports(1, &m_viewport);

    return true;
}

bool DX11Renderer::CreateDepthBuffer(int width, int height)
{
    // Depth 텍스처 생성
    D3D11_TEXTURE2D_DESC dd = {};
    dd.Width = (UINT)width;
    dd.Height = (UINT)height;
    dd.MipLevels = 1;
    dd.ArraySize = 1;
    dd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dd.SampleDesc.Count = 1;
    dd.Usage = D3D11_USAGE_DEFAULT;
    dd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTex;
    HRESULT hr = m_device->CreateTexture2D(&dd, nullptr, &depthTex);
    if (FAILED(hr)) return false;

    hr = m_device->CreateDepthStencilView(depthTex.Get(), nullptr, &m_dsv);
    return SUCCEEDED(hr);
}

void DX11Renderer::BeginFrame(float r, float g, float b)
{
    // RTV + DSV 바인딩 (매 프레임 재확인)
    m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());
    m_context->RSSetViewports(1, &m_viewport);

    // 화면 클리어
    float color[4] = { r, g, b, 1.0f };
    m_context->ClearRenderTargetView(m_rtv.Get(), color);
    m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}

bool DX11Renderer::CompileShader(const wchar_t* filePath, const char* entry, const char* target, ID3DBlob** ppBlob)
{
    if (_waccess(filePath, 0) == -1)
    {
        OutputDebugStringW(L"[ERROR] Shader File Not Found: ");
        OutputDebugStringW(filePath);
        return false;
    }

    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3DCompileFromFile( filePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,entry, target,
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, ppBlob, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        return false;
    }

    return true;
}


void DX11Renderer::Flip()
{
    // VSync On (0 → Off)
    m_swapChain->Present(1, 0);
}
