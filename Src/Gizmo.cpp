#include "Gizmo.h"
#include "GameEngine.h"
#include "RenderManager.h"
#include "DX11Renderer.h"
#include "InputHandler.h"
#include "EditorUI.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"

#include <d3dcompiler.h>

using namespace DirectX;


#pragma region  Gizmo 전용 인라인 셰이더 (파일 없이 컴파일)

static const char* GIZMO_SHADER_SRC = R"(
cbuffer CBMatrix : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};
cbuffer CBColor : register(b1)
{
    float4 Color;
};
struct VSIn  { float3 pos : POSITION; };
struct VSOut { float4 pos : SV_POSITION; };
 
VSOut VS(VSIn i)
{
    VSOut o;
    float4 p = float4(i.pos, 1.0f);
    p = mul(p, World);
    p = mul(p, View);
    p = mul(p, Projection);
    o.pos = p;
    return o;
}
float4 PS(VSOut i) : SV_TARGET { return Color; }
)";

namespace CSGizmo
{
	struct GizmoVertex { float x, y, z; };
	struct CBMatrix { XMMATRIX World, View, Projection; };
	struct CBColor { XMFLOAT4 Color; };
}

#pragma endregion

Gizmo::Gizmo() {}

Gizmo::~Gizmo() {}

bool Gizmo::Init(DX11Renderer* render)
{
	m_initialized = InitShader(render) && InitArrowBuffers(render) && InitCBuffers(render);
	return m_initialized;
}

void Gizmo::Update(float deltaTime)
{
	if (!m_target || !m_initialized) return;

	InputHandler* input = InputHandler::Instance();
	XMVECTOR rayOrigin, rayDir;
	CalcMouseRay(rayOrigin, rayDir);

	// ImGui가 마우스를 사용 중이면 무시
	// (imgui.h include 없이 간단히 처리: 좌클릭 && !ImGui WantCaptureMouse)
	bool lmbDown = input->IsKeyDown(VK_LBUTTON);
	bool lmbOne = input->IsKeyOneDown(VK_LBUTTON);

	if (!m_isDragging)
	{
		// 마우스 호버 → 어떤 축인지 하이라이트
		m_activeAxis = PickAxis(rayOrigin, rayDir);

		// 2. 축을 선택한 상태에서 클릭하면 드래그 시작
		if (lmbOne && m_activeAxis != Axis::None)
		{
			// 드래그 시작
			m_isDragging = true;
			m_dragStartRayOrigin = rayOrigin;
			m_dragStartRayDir = rayDir;


			m_dragStartT = CalcDragDelta(rayOrigin, rayDir, m_activeAxis);
		}
	}
	else
	{
		if (!lmbDown)
		{
			// 드래그 종료
			m_isDragging = false;
			m_activeAxis = Axis::None;
			return;
		}

		// 드래그 중 — 이동량 계산
		float curT = CalcDragDelta(rayOrigin, rayDir, m_activeAxis);
		float delta = curT - m_dragStartT;
		m_dragStartT = curT; // 다음 프레임 기준 갱신 (누적)

		Vector3 pos = m_target->GetTransform()->GetPosition();
		if (m_activeAxis == Axis::X) pos.x += delta;
		else if (m_activeAxis == Axis::Y) pos.y += delta;
		else if (m_activeAxis == Axis::Z) pos.z += delta;
		m_target->GetTransform()->SetPosition(pos);
	}
}

void Gizmo::Render()
{
	if (!m_target || !m_initialized) return;

	Vector3 pos = m_target->GetTransform()->GetPosition();
	XMFLOAT3 o = { pos.x, pos.y, pos.z };

	// 호버/드래그 중인 축은 밝게
	auto col = [&](Axis a, XMFLOAT4 normal, float bright = 1.4f) -> XMFLOAT4
		{
			if (m_activeAxis == a)
				return { normal.x * bright, normal.y * bright, normal.z * bright, 1.f };
			return normal;
		};

	DrawArrow(o, { pos.x + ARROW_LENGTH, pos.y,               pos.z }, col(Axis::X, { 2,0,0,2 })); // X 빨강
	DrawArrow(o, { pos.x,               pos.y + ARROW_LENGTH, pos.z }, col(Axis::Y, { 0,2,0,2 })); // Y 초록
	DrawArrow(o, { pos.x,               pos.y,               pos.z + ARROW_LENGTH }, col(Axis::Z, { 0,0,2,2 })); // Z 파랑
}

void Gizmo::CalcMouseRay(DirectX::XMVECTOR& outOrigin, DirectX::XMVECTOR& outDir) const
{
	Camera* cam = GameEngine::Instance()->GetMainCamera();
	if (!cam) return;

	auto* renderer = GameEngine::Instance()->GetRenderManager()->GetRenderer();
	float W = (float)renderer->GetWidth();
	float H = (float)renderer->GetHeight();

	POINT mp = InputHandler::Instance()->GetMousePos();
	float ndcX = (2.f * mp.x / W) - 1.f;
	float ndcY = (-2.f * mp.y / H) + 1.f;

	XMMATRIX proj = cam->GetProjectionMatrix();
	XMMATRIX view = cam->GetViewMatrix();
	XMMATRIX invView = XMMatrixInverse(nullptr, view);

	// NDC → 뷰 공간
	float tanHalfFovX = 1.f / XMVectorGetX(proj.r[0]);
	float tanHalfFovY = 1.f / XMVectorGetY(proj.r[1]);
	XMVECTOR rayView = XMVectorSet(ndcX * tanHalfFovX, ndcY * tanHalfFovY, 1.f, 0.f);

	// 뷰 공간 → 월드 공간
	outDir = XMVector3Normalize(XMVector3TransformNormal(rayView, invView));
	outOrigin = invView.r[3]; // 카메라 위치
}

Gizmo::Axis Gizmo::PickAxis(const DirectX::XMVECTOR& rayOrigin, const DirectX::XMVECTOR& rayDir) const
{
	if (!m_target) return Axis::None;

	Vector3 objPos = m_target->GetTransform()->GetPosition();
	XMVECTOR origin = XMVectorSet(objPos.x, objPos.y, objPos.z, 1.f);

	// 각 축 화살표 끝점
	XMVECTOR ends[3] = {
		XMVectorSet(objPos.x + ARROW_LENGTH, objPos.y,               objPos.z,               1.f), // X
		XMVectorSet(objPos.x,               objPos.y + ARROW_LENGTH, objPos.z,               1.f), // Y
		XMVectorSet(objPos.x,               objPos.y,               objPos.z + ARROW_LENGTH, 1.f), // Z
	};

	float minDist = AXIS_HIT_RADIUS;
	Axis  best = Axis::None;


	for (int i = 0; i < 3; i++)
	{
		// 레이와 화살표 선분 사이 최단 거리 계산
		XMVECTOR axisDir = XMVector3Normalize(XMVectorSubtract(ends[i], origin));
		XMVECTOR w = XMVectorSubtract(rayOrigin, origin);

		float a = XMVectorGetX(XMVector3Dot(rayDir, rayDir));
		float b = XMVectorGetX(XMVector3Dot(rayDir, axisDir));
		float c = XMVectorGetX(XMVector3Dot(axisDir, axisDir));
		float d = XMVectorGetX(XMVector3Dot(rayDir, w));
		float e = XMVectorGetX(XMVector3Dot(axisDir, w));

		float denom = a * c - b * b;
		if (fabsf(denom) < 1e-6f) continue;

		float t = (b * e - c * d) / denom;
		float s = (a * e - b * d) / denom;

		// 화살표 길이 범위 안에 있어야 함
		if (s < 0.f || s > ARROW_LENGTH) continue;

		XMVECTOR p1 = XMVectorAdd(rayOrigin, XMVectorScale(rayDir, t));
		XMVECTOR p2 = XMVectorAdd(origin, XMVectorScale(axisDir, s));
		float dist = XMVectorGetX(XMVector3Length(XMVectorSubtract(p1, p2)));

		if (dist < minDist)
		{
			minDist = dist;
			best = (Axis)(i + 1);
		}
	}
	return best;
}

// ── 드래그 이동량 계산 ────────────────────────────────────────────────
float Gizmo::CalcDragDelta(const DirectX::XMVECTOR& rayOrigin, const DirectX::XMVECTOR& rayDir, Axis axis) const
{
	if (!m_target) return 0.f;

	Vector3 objPos = m_target->GetTransform()->GetPosition();
	XMVECTOR origin = XMVectorSet(objPos.x, objPos.y, objPos.z, 1.f);

	// 축 방향
	XMVECTOR axisDir = XMVectorSet(
		axis == Axis::X ? 1.f : 0.f,
		axis == Axis::Y ? 1.f : 0.f,
		axis == Axis::Z ? 1.f : 0.f,
		0.f);

	// 레이와 축 직선 최근접점의 t 계산
	XMVECTOR w = XMVectorSubtract(rayOrigin, origin);
	float a = XMVectorGetX(XMVector3Dot(rayDir, rayDir));
	float b = XMVectorGetX(XMVector3Dot(rayDir, axisDir));
	float c = XMVectorGetX(XMVector3Dot(axisDir, axisDir));
	float d = XMVectorGetX(XMVector3Dot(rayDir, w));
	float e = XMVectorGetX(XMVector3Dot(axisDir, w));

	float denom = a * c - b * b;
	if (fabsf(denom) < 1e-6f) return 0.f;

	return (a * e - b * d) / denom; // 축 위의 투영 거리
}

void Gizmo::DrawArrow(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& color)
{
	auto* renderer = GameEngine::Instance()->GetRenderManager()->GetRenderer();
	auto* ctx = renderer->GetContext();

	// 버텍스 업데이트
	CSGizmo::GizmoVertex verts[2] = {
		{start.x, start.y, start.z},
		{end.x,   end.y,   end.z  }
	};

	D3D11_MAPPED_SUBRESOURCE mapped;
	ctx->Map(m_vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, verts, sizeof(verts));
	ctx->Unmap(m_vb.Get(), 0);

	// 카메라 행렬
	Camera* cam = GameEngine::Instance()->GetMainCamera();
	if (!cam) return;

	CSGizmo::CBMatrix cbm;
	cbm.World = XMMatrixTranspose(XMMatrixIdentity());
	cbm.View = XMMatrixTranspose(cam->GetViewMatrix());
	cbm.Projection = XMMatrixTranspose(cam->GetProjectionMatrix());

	ctx->Map(m_cbMatrix.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &cbm, sizeof(CSGizmo::CBMatrix));
	ctx->Unmap(m_cbMatrix.Get(), 0);

	CSGizmo::CBColor cbc = { color };
	ctx->Map(m_cbColor.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &cbc, sizeof(CSGizmo::CBColor));
	ctx->Unmap(m_cbColor.Get(), 0);

	UINT stride = sizeof(CSGizmo::GizmoVertex), offset = 0;
	ctx->IASetVertexBuffers(0, 1, m_vb.GetAddressOf(), &stride, &offset);
	ctx->IASetInputLayout(m_inputLayout.Get());
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	ctx->VSSetShader(m_vs.Get(), nullptr, 0);
	ctx->PSSetShader(m_ps.Get(), nullptr, 0);
	ctx->VSSetConstantBuffers(0, 1, m_cbMatrix.GetAddressOf());
	ctx->PSSetConstantBuffers(1, 1, m_cbColor.GetAddressOf());
	ctx->Draw(2, 0);
}

bool Gizmo::InitShader(DX11Renderer* render)
{

	auto* dev = render->GetDevice();
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob, errBlob;
	HRESULT hr = D3DCompile(GIZMO_SHADER_SRC, strlen(GIZMO_SHADER_SRC), nullptr, nullptr, nullptr, "VS", "vs_5_0", D3DCOMPILE_DEBUG, 0, &vsBlob, &errBlob);
	if (FAILED(hr)) { if (errBlob) OutputDebugStringA((char*)errBlob->GetBufferPointer()); return false; }

	hr = D3DCompile(GIZMO_SHADER_SRC, strlen(GIZMO_SHADER_SRC), nullptr, nullptr, nullptr, "PS", "ps_5_0", D3DCOMPILE_DEBUG, 0, &psBlob, &errBlob);
	if (FAILED(hr)) return false;

	dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vs);
	dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_ps);

	D3D11_INPUT_ELEMENT_DESC layout[] = { {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0} };
	dev->CreateInputLayout(layout, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);

	return true;
}

bool Gizmo::InitArrowBuffers(DX11Renderer* render)
{
	auto* dev = render->GetDevice();

	// 라인 2개의 점 (DrawArrow에서 매번 업데이트)
	CSGizmo::GizmoVertex verts[2] = { {0,0,0},{1,0,0} };
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(verts);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA sd = { verts };
	return SUCCEEDED(dev->CreateBuffer(&bd, &sd, &m_vb));
}

bool Gizmo::InitCBuffers(DX11Renderer* render)
{
	auto* dev = render->GetDevice();

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bd.ByteWidth = sizeof(CSGizmo::CBMatrix);
	if (FAILED(dev->CreateBuffer(&bd, nullptr, &m_cbMatrix))) return false;

	bd.ByteWidth = sizeof(CSGizmo::CBColor);
	if (FAILED(dev->CreateBuffer(&bd, nullptr, &m_cbColor))) return false;

	return true;
}
