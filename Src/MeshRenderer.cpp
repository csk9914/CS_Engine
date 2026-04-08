#include "MeshRenderer.h"
#include "GameEngine.h"
#include "RenderManager.h"
#include "DX11Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

#include "Mesh.h"
#include "MeshFilter.h"

#include "imgui/imgui.h"

using namespace DirectX;

MeshRenderer::MeshRenderer(const std::string& name) : Component(name){}

MeshRenderer::~MeshRenderer(){}

void MeshRenderer::Awake()
{
    m_initialized =  InitShader() && InitCBuffers();
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

    // 1. MeshFilter 컴포넌트 가져오기
    auto* filter = GetGameObject()->GetComponent<MeshFilter>();
    if (!filter || !filter->GetMesh()) 
        return; // 그릴 메쉬가 없으면 리턴
    Mesh* mesh = filter->GetMesh();


    Camera* cam = GameEngine::Instance()->GetRenderCamera();
    if (!cam) {return;}

    auto* renderer = GameEngine::Instance()->GetRenderManager()->GetRenderer();
    auto* ctx = renderer->GetContext();

    // ── Camera 컴포넌트에서 View/Proj 가져오기 ───────────────────────
    XMMATRIX view = cam->GetViewMatrix();
    XMMATRIX proj = cam->GetProjectionMatrix();
       
    // ── Constant Buffer 업데이트 ────────────────────────────────────
    CBMatrix cbm;
    cbm.World = XMMatrixTranspose(GetGameObject()->GetTransform()->GetWorldMatrix());
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
    mesh->Bind(ctx);

    ctx->IASetInputLayout(m_inputLayout.Get());
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(m_vs.Get(), nullptr, 0);
    ctx->PSSetShader(m_ps.Get(), nullptr, 0);
    ctx->VSSetConstantBuffers(0, 1, m_cbMatrix.GetAddressOf());
    ctx->PSSetConstantBuffers(1, 1, m_cbColor.GetAddressOf());

    // DrawIndexed 호출 시 메쉬가 가진 인덱스 개수 사용
    ctx->DrawIndexed(mesh->GetIndexCount(), 0, 0);
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


void MeshRenderer::OnEditorGUI()
{
    float col[4] = { m_color.x, m_color.y,m_color.z, m_color.w };

    if (ImGui::ColorEdit4("Color", col)) SetColor(col[0], col[1], col[2], col[3]);
}