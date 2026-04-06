#include "RenderManager.h"
#include "DX11Renderer.h"
#include "EditorUI.h"
#include "Gizmo.h"
#include "SpriteRenderer.h"
#include "MeshRenderer.h"
#include "InputHandler.h"
#include "GameEngine.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "imgui.h"

#include <DirectXMath.h>
using namespace DirectX;

RenderManager::RenderManager(HWND hWnd, int width, int height)
{
	m_renderer = DX11Renderer::Create(hWnd, width, height);

	// 4. EditorUI (ImGui)
	m_editorUI = std::make_unique<EditorUI>();
	m_editorUI->Init(hWnd, m_renderer->GetDevice(), m_renderer->GetContext());

	if (!m_renderer->GetDevice())
	{
		int a = 10;
	}

	if (!m_renderer->GetContext())
	{
		int a = 10;
	}
	// Gizmo
	m_gizmo = std::make_unique<Gizmo>();
	m_gizmo->Init(m_renderer.get());

}

RenderManager::~RenderManager() {}

void RenderManager::RegisterSprite(SpriteRenderer* sprite)
{
	m_sprites.push_back(sprite);
}

void RenderManager::UnregisterSprite(SpriteRenderer* sprite)
{
	// 벡터에서 해당 스프라이트 제거
	// std::remove는 제거할 요소를 벡터의 끝으로 이동시키고, 실제로 제거된 요소는 벡터의 끝에 남아있게 됨
	// 따라서 erase를 사용하여 벡터의 끝에서 제거된 요소들을 삭제
	// 원본이 삭제되는 것은 아님!!
	m_sprites.erase(std::remove(m_sprites.begin(), m_sprites.end(), sprite), m_sprites.end());
}

void RenderManager::RegisterMesh(MeshRenderer* mesh)
{
	m_meshes.push_back(mesh);
}

void RenderManager::UnregisterMesh(MeshRenderer* mesh)
{
	m_meshes.erase(
		std::remove(m_meshes.begin(), m_meshes.end(), mesh),
		m_meshes.end());
}

void RenderManager::Update()
{
	// 5. Gizmo — 선택된 오브젝트 추적
	if (m_gizmo)
	{
		m_gizmo->SetTarget(m_editorUI->GetSelected());
		m_gizmo->Update(0.f); // dt는 GameEngine에서 받아오지 않아도 됨
		//m_gizmo->Render();
	}
}

void RenderManager::RenderAll()
{
	if (!m_renderer) return;

	// 1. 프레임 시작 (화면 클리어)
	m_renderer->BeginFrame(0.05f, 0.05f, 0.1f);

	// 2. MeshRenderer 전부 그리기 (나중에 orderInLayer 정렬 추가 가능)
	for (MeshRenderer* mesh : m_meshes)
		if (mesh) mesh->Render();

	// 모든 스프라이트 그리기
	for (const auto& sprite : m_sprites)
	{
		if (sprite) sprite->Render();
	}

	// 4. Ray cast 오브젝트 선택
	TryPickObject();

	// 5. Gizmo — 선택된 오브젝트 추적
	if (m_gizmo)
	{
		//m_gizmo->SetTarget(m_editorUI->GetSelected());
		//m_gizmo->Update(0.f); // dt는 GameEngine에서 받아오지 않아도 됨
		m_gizmo->Render();
	}

	// ── ImGui (씬 위에 덮어서 그림) ─────────────────────────────
	m_editorUI->BeginFrame();
	m_editorUI->Draw(GameEngine::Instance()->GetCurrentScene()); // IGame에서 씬 가져옴
	m_editorUI->EndFrame();

	// 버퍼 전환
	m_renderer->Flip();
}

void RenderManager::Release()
{
	if (m_editorUI)
		m_editorUI->Shutdown();
}

void RenderManager::TryPickObject()
{
	// ImGui가 마우스를 가져가고 있으면 무시
	if (ImGui::GetIO().WantCaptureMouse) return;

	// 좌클릭 순간만
	InputHandler* input = InputHandler::Instance();
	if (!input->IsKeyOneDown(VK_LBUTTON)) return;

	Camera* cam = GameEngine::Instance()->GetMainCamera();
	if (!cam) return;

	auto* renderer = m_renderer.get();
	float W = (float)renderer->GetWidth();
	float H = (float)renderer->GetHeight();

	// 마우스 → NDC → 월드 레이
	POINT mp = input->GetMousePos();
	float ndcX = (2.f * mp.x / W) - 1.f;
	float ndcY = -(2.f * mp.y / H) + 1.f;

	XMMATRIX proj = cam->GetProjectionMatrix();
	XMMATRIX view = cam->GetViewMatrix();
	XMMATRIX invView = XMMatrixInverse(nullptr, view);

	float tanHalfFovX = 1.f / XMVectorGetX(proj.r[0]);
	float tanHalfFovY = 1.f / XMVectorGetY(proj.r[1]);

	XMVECTOR rayView = XMVectorSet(ndcX * tanHalfFovX,
		ndcY * tanHalfFovY, 1.f, 0.f);
	XMVECTOR rayDir = XMVector3Normalize(
		XMVector3TransformNormal(rayView, invView));
	XMVECTOR rayOrigin = invView.r[3];

	// 등록된 MeshRenderer의 AABB와 교차 판정
	float        minT = FLT_MAX;
	GameObject* picked = nullptr;
	float        boxHalf = 0.5f; // 기본 큐브 크기

	for (MeshRenderer* mr : m_meshes)
	{
		if (!mr) continue;
		GameObject* obj = mr->GetOwner();
		if (!obj || !obj->GetActive()) continue;

		Vector3 pos = obj->GetTransform()->GetPosition();
		Vector3 scale = obj->GetTransform()->GetScale();

		// 스케일을 고려한 AABB
		XMVECTOR bMin = XMVectorSet(pos.x - boxHalf * scale.x,
			pos.y - boxHalf * scale.y,
			pos.z - boxHalf * scale.z, 0.f);
		XMVECTOR bMax = XMVectorSet(pos.x + boxHalf * scale.x,
			pos.y + boxHalf * scale.y,
			pos.z + boxHalf * scale.z, 0.f);

		// Ray-AABB 교차 (slab method)
		XMVECTOR invDir = XMVectorReciprocal(rayDir);
		XMVECTOR t0 = XMVectorMultiply(XMVectorSubtract(bMin, rayOrigin), invDir);
		XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(bMax, rayOrigin), invDir);

		XMVECTOR tMin = XMVectorMin(t0, t1);
		XMVECTOR tMax = XMVectorMax(t0, t1);

		float tEnter = std::max({ XMVectorGetX(tMin), XMVectorGetY(tMin), XMVectorGetZ(tMin) });
		float tExit = std::min({ XMVectorGetX(tMax), XMVectorGetY(tMax), XMVectorGetZ(tMax) });

		if (tEnter <= tExit && tExit > 0.f && tEnter < minT)
		{
			minT = tEnter;
			picked = obj;
		}
	}

	// 결과를 EditorUI에 반영 (Hierarchy와 동기화)
	m_editorUI->SetSelected(picked);
}
