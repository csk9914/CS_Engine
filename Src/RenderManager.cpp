#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <DirectXMath.h>

#include "RenderManager.h"
#include "DX11Renderer.h"
#include "EditorUI.h"
#include "SpriteRenderer.h"
#include "MeshRenderer.h"
#include "InputHandler.h"
#include "GameEngine.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "RenderPipeline.h"
#include "imgui.h"
#include "imgui/ImGuizmo.h"
#include "GameApp.h"

using namespace DirectX;

RenderManager::RenderManager(HWND hWnd, int width, int height)
{
	m_renderer = DX11Renderer::Create(hWnd, width, height);

	m_pipeline = std::make_unique<RenderPipeline>(m_renderer.get());

	m_editorUI = std::make_unique<EditorUI>();
	m_editorUI->Init(hWnd, m_renderer.get());
}

RenderManager::~RenderManager() {}

void RenderManager::RegisterSprite(SpriteRenderer* s) { m_sprites.push_back(s); }

void RenderManager::UnregisterSprite(SpriteRenderer* s)
{
	m_sprites.erase(std::remove(m_sprites.begin(), m_sprites.end(), s), m_sprites.end());
}

void RenderManager::RegisterMesh(MeshRenderer* mesh)
{
	if (std::find(m_meshes.begin(), m_meshes.end(), mesh) == m_meshes.end())
	{

		m_meshes.push_back(mesh);
	}
}

void RenderManager::UnregisterMesh(MeshRenderer* m)
{
	m_meshes.erase(std::remove(m_meshes.begin(), m_meshes.end(), m), m_meshes.end());
}

void RenderManager::RenderAll(float deltaTime)
{
	if (!m_renderer || !m_pipeline) return;

	// 각 뷰가 Pipeline에 렌더 요청 Push 
	m_editorUI->Update(deltaTime, m_pipeline.get());

	//  Pipeline 실행 → SceneRTV, GameRTV에 그리기 
	m_pipeline->Execute(m_meshes);

	// Raycast는 ImGui 렌더 사이에 호출해야
	// ImGuizmo::IsOver() / IsUsing() 값이 정확함
	Raycast();


	// 메인 백버퍼 클리어 
	m_renderer->BeginFrame(0.1f, 0.1f, 0.1f);

	// ImGui 렌더 (SceneView/GameView RTT를 Image로 출력) 
	m_editorUI->BeginFrame();

	m_editorUI->Draw(GameEngine::Instance()->GetGameApp(), m_renderer->GetSceneSRV(), m_renderer->GetGameSRV());

	m_editorUI->EndFrame();

	// Present
	m_renderer->Flip();
}

void RenderManager::Release()
{
	if (m_editorUI) m_editorUI->Shutdown();
}

void RenderManager::Raycast()
{
	InputHandler* input = InputHandler::Instance();

	// IsUsing() = 드래그 중, IsOver() = 마우스가 핸들 위에 있음
	if (ImGuizmo::IsUsing() || ImGuizmo::IsOver()) return;

	// 좌클릭 순간(프레임에 한 번)만 처리
	if (!input->IsKeyOneDown(VK_LBUTTON)) return;

	// ImGui가 이 클릭을 가져갔으면 무시 (패널 클릭 등)
	//if (ImGui::GetIO().WantCaptureMouse) return;

	ImVec2 sceneViewPos = m_editorUI->GetSceneViewPos();
	ImVec2 sceneViewSize = m_editorUI->GetSceneViewSize();
	POINT mousePos = input->GetMousePos();

	// SceneView 영역 밖이면 무시
	if (mousePos.x < sceneViewPos.x || mousePos.x > sceneViewPos.x + sceneViewSize.x ||
		mousePos.y < sceneViewPos.y || mousePos.y > sceneViewPos.y + sceneViewSize.y) return;

	float relX = (float)mousePos.x - sceneViewPos.x;
	float relY = (float)mousePos.y - sceneViewPos.y;

	Camera* cam = GameEngine::Instance()->GetEditorCamera();
	if (!cam) return;

	Ray ray = cam->ScreenPointToRay({ relX, relY }, { sceneViewSize.x, sceneViewSize.y });
	XMVECTOR rayOrigin = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&ray.origin));
	XMVECTOR rayDir = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&ray.direction));

	GameObject* picked = nullptr;
	float minT = FLT_MAX;
	float half = 0.5f;

	for (MeshRenderer* meshRender : m_meshes)
	{
		if (!meshRender) continue;
		GameObject* obj = meshRender->GetGameObject();
		if (!obj || !obj->GetActive()) continue;

		Vector3 pos = obj->GetTransform()->GetPosition();
		Vector3 scale = obj->GetTransform()->GetScale();

		XMVECTOR bMin = XMVectorSet(pos.x - half * scale.x, pos.y - half * scale.y, pos.z - half * scale.z, 0.f);
		XMVECTOR bMax = XMVectorSet(pos.x + half * scale.x, pos.y + half * scale.y, pos.z + half * scale.z, 0.f);

		XMVECTOR invD = XMVectorReciprocal(rayDir);
		XMVECTOR t0 = XMVectorMultiply(XMVectorSubtract(bMin, rayOrigin), invD);
		XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(bMax, rayOrigin), invD);

		XMVECTOR tMin = XMVectorMin(t0, t1);
		XMVECTOR tMax = XMVectorMax(t0, t1);

		float tE = std::max({ XMVectorGetX(tMin), XMVectorGetY(tMin), XMVectorGetZ(tMin) });
		float tX = std::min({ XMVectorGetX(tMax), XMVectorGetY(tMax), XMVectorGetZ(tMax) });

		if (tE <= tX && tX > 0.f && tE < minT)
		{
			minT = tE;
			picked = obj;
		}
	}

	// picked != nullptr → 선택 변경
	// picked == nullptr → 선택 해제 (아무것도 없는 곳 클릭)
	m_editorUI->SetSelected(picked); // nullptr이면 선택 해제
}
