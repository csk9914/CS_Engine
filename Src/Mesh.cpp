#include "Mesh.h"
#include "DX11Renderer.h"
#include "GameEngine.h"
#include "RenderManager.h"

Mesh::Mesh(const MeshData& data, const std::string name) : Mesh(name)
{
    Create(data);
}

bool Mesh::Create(const MeshData& data)
{
    auto* device = GameEngine::Instance()->GetRenderManager()->GetRenderer()->GetDevice();

    // 버텍스 버퍼
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = (UINT)(sizeof(Vertex) * data.vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vsd = {};
    vsd.pSysMem = data.vertices.data();

    if (FAILED(device->CreateBuffer(&vbDesc, &vsd, &m_vBuffer))) return false;


    // 인덱스 버퍼
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = (UINT)(sizeof(UINT) * data.indices.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = data.indices.data();

    if (FAILED(device->CreateBuffer(&ibDesc, &isd, &m_iBuffer))) return false;

    m_indexCount = (UINT)data.indices.size();

    // 바운딩 정보 저장
    data.GetBounds(m_min, m_max);

    return true;
}

// 파이프라인 바인딩
void Mesh::Bind(ID3D11DeviceContext* ctx)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    ctx->IASetVertexBuffers(0, 1, m_vBuffer.GetAddressOf(), &stride, &offset);
    ctx->IASetIndexBuffer(m_iBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}
