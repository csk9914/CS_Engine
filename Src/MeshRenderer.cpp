#include "MeshRenderer.h"
#include "GameEngine.h"
#include "RenderManager.h"
#include "DX11Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

using namespace DirectX;

// 큐브 버텍스 (위치만)
struct Vertex { float x, y, z; };

static const Vertex CUBE_VERTS[] =
{
    // 앞면
    {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
    // 뒷면
    { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f},
    {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f},
};

static const UINT CUBE_INDICES[] =
{
    0,1,2, 0,2,3, // 앞
    4,5,6, 4,6,7, // 뒤
    5,0,3, 5,3,6, // 왼쪽
    1,4,7, 1,7,2, // 오른쪽
    5,4,1, 5,1,0, // 위
    3,2,7, 3,7,6, // 아래
};

MeshRenderer::MeshRenderer(){}

MeshRenderer::~MeshRenderer(){}

void MeshRenderer::Awake()
{
    m_initialized = InitCube() && InitShader() && InitCBuffers();
}

void MeshRenderer::OnEnable()
{
    GameEngine::Instance()->GetRenderManager()->RegisterMesh(this);
}

void MeshRenderer::OnDisable()
{
    GameEngine::Instance()->GetRenderManager()->UnregisterMesh(this);
}

void MeshRenderer::OnDestroy()
{
    GameEngine::Instance()->GetRenderManager()->UnregisterMesh(this);
}

void MeshRenderer::Render()
{
    if (!m_initialized) return;

    Camera* cam = GameEngine::Instance()->GetRenderCamera();
    if (!cam) {return;}

    auto* renderer = GameEngine::Instance()->GetRenderManager()->GetRenderer();
    auto* ctx = renderer->GetContext();

    // ── Camera 컴포넌트에서 View/Proj 가져오기 ───────────────────────
   // (없으면 기본값 사용)
    XMMATRIX view = cam->GetViewMatrix();
    XMMATRIX proj = cam->GetProjectionMatrix();
       
    // ── Constant Buffer 업데이트 ────────────────────────────────────
    CBMatrix cbm;
    cbm.World = XMMatrixTranspose(GetOwner()->GetTransform()->GetWorldMatrix());
    cbm.View = XMMatrixTranspose(view);
    cbm.Projection = XMMatrixTranspose(proj);

    D3D11_MAPPED_SUBRESOURCE mapped;
    ctx->Map(m_cbMatrix.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &cbm, sizeof(cbm));
    ctx->Unmap(m_cbMatrix.Get(), 0);

    CBColor cbc = { m_color };
    ctx->Map(m_cbColor.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &cbc, sizeof(cbc));
    ctx->Unmap(m_cbColor.Get(), 0);

    // 파이프라인 바인딩
    UINT stride = sizeof(Vertex), offset = 0;
    ctx->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    ctx->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetInputLayout(m_inputLayout.Get());
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(m_vs.Get(), nullptr, 0);
    ctx->PSSetShader(m_ps.Get(), nullptr, 0);
    ctx->VSSetConstantBuffers(0, 1, m_cbMatrix.GetAddressOf());
    ctx->PSSetConstantBuffers(1, 1, m_cbColor.GetAddressOf());

    // 드로우
    ctx->DrawIndexed(m_indexCount, 0, 0);
}

bool MeshRenderer::InitCube()
{
    auto* device = GameEngine::Instance()->GetRenderManager()->GetRenderer()->GetDevice();

    // 버텍스 버퍼
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CUBE_VERTS);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = CUBE_VERTS;
    if (FAILED(device->CreateBuffer(&bd, &sd, &m_vertexBuffer))) return false;

    // 인덱스 버퍼
    bd.ByteWidth = sizeof(CUBE_INDICES);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    sd.pSysMem = CUBE_INDICES;
    if (FAILED(device->CreateBuffer(&bd, &sd, &m_indexBuffer))) return false;

    m_indexCount = ARRAYSIZE(CUBE_INDICES);

	return true;
}

bool MeshRenderer::InitShader()
{
    auto* renderer = GameEngine::Instance()->GetRenderManager()->GetRenderer();
    auto* device = renderer->GetDevice();

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob;
    if (!renderer->CompileShader(L"Shaders/Basic.hlsl", "VS", "vs_5_0", &vsBlob))
        return false;
    
    if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vs))) return false;

    if (!renderer->CompileShader(L"Shaders/Basic.hlsl", "PS", "ps_5_0", &psBlob))
        return false;

    if (FAILED(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_ps))) return false;

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
          D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    if (FAILED(device->CreateInputLayout(layout, 1,vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout))) return false;

    return true;
}

bool MeshRenderer::InitCBuffers()
{
    auto* device = GameEngine::Instance()->GetRenderManager()->GetRenderer()->GetDevice();

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // CBMatrix
    bd.ByteWidth = sizeof(CBMatrix);
    if (FAILED(device->CreateBuffer(&bd, nullptr, &m_cbMatrix))) return false;

    // CBColor
    bd.ByteWidth = sizeof(CBColor);
    if (FAILED(device->CreateBuffer(&bd, nullptr, &m_cbColor))) return false;
	
    return true;
}
