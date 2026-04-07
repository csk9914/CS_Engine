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

using namespace DirectX;

RenderManager::RenderManager(HWND hWnd, int width, int height)
{
    m_renderer = DX11Renderer::Create(hWnd, width, height);

    m_pipeline = std::make_unique<RenderPipeline>(m_renderer.get());

    m_editorUI = std::make_unique<EditorUI>();
    m_editorUI->Init(hWnd, m_renderer->GetDevice(), m_renderer->GetContext());
}

RenderManager::~RenderManager() {}

void RenderManager::RegisterSprite(SpriteRenderer* s)  { m_sprites.push_back(s); }
void RenderManager::UnregisterSprite(SpriteRenderer* s)
{
    m_sprites.erase(std::remove(m_sprites.begin(), m_sprites.end(), s), m_sprites.end());
}
void RenderManager::RegisterMesh(MeshRenderer* m)   { m_meshes.push_back(m); }
void RenderManager::UnregisterMesh(MeshRenderer* m)
{
    m_meshes.erase(std::remove(m_meshes.begin(), m_meshes.end(), m), m_meshes.end());
}

void RenderManager::RenderAll(float deltaTime)
{
    if (!m_renderer || !m_pipeline) return;

    // ── 1. 각 뷰가 Pipeline에 렌더 요청 Push ────────────────────────
    m_editorUI->Update(deltaTime, m_pipeline.get(), m_renderer.get());

    // ── 2. Pipeline 실행 → SceneRTV, GameRTV에 그리기 ───────────────
    m_pipeline->Execute(m_meshes);

    // ── 3. 메인 백버퍼 클리어 ────────────────────────────────────────
    m_renderer->BeginFrame(0.1f, 0.1f, 0.1f);

    // ── 4. Ray cast (ImGui NewFrame 전에 해야 WantCaptureMouse 정확) ─
    Raycast();

    // ── 5. ImGui 렌더 (SceneView/GameView RTT를 Image로 출력) ────────
    m_editorUI->BeginFrame();
    m_editorUI->Draw(
        GameEngine::Instance()->GetCurrentScene(),
        m_renderer->GetSceneSRV(),
        m_renderer->GetGameSRV());
    m_editorUI->EndFrame();

    // ── 6. Present ───────────────────────────────────────────────────
    m_renderer->Flip();
}

void RenderManager::Release()
{
    if (m_editorUI) m_editorUI->Shutdown();
}

void RenderManager::Raycast()
{
    InputHandler* input = InputHandler::Instance();
    if (!input->IsKeyOneDown(VK_LBUTTON)) return;

    // Gizmo 조작 중이면 선택 변경 무시
    if (m_editorUI->GetSelected() && (ImGuizmo::IsOver() || ImGuizmo::IsUsing()))
        return;

    ImVec2 sPos  = m_editorUI->GetSceneViewPos();
    ImVec2 sSize = m_editorUI->GetSceneViewSize();
    POINT  mp    = input->GetMousePos();

    // SceneView 영역 밖이면 무시
    if (mp.x < sPos.x || mp.x > sPos.x + sSize.x ||
        mp.y < sPos.y || mp.y > sPos.y + sSize.y) return;

    float relX = (float)mp.x - sPos.x;
    float relY = (float)mp.y - sPos.y;

    Camera* cam = GameEngine::Instance()->GetEditorCamera();
    if (!cam) return;

    Ray ray = cam->ScreenPointToRay({relX, relY}, {sSize.x, sSize.y});
    XMVECTOR rayOrigin = XMLoadFloat3((XMFLOAT3*)&ray.origin);
    XMVECTOR rayDir    = XMLoadFloat3((XMFLOAT3*)&ray.direction);

    float       minT   = FLT_MAX;
    GameObject* picked = nullptr;
    float       half   = 0.5f;

    for (MeshRenderer* mr : m_meshes)
    {
        if (!mr) continue;
        GameObject* obj = mr->GetOwner();
        if (!obj || !obj->GetActive()) continue;

        Vector3 pos   = obj->GetTransform()->GetPosition();
        Vector3 scale = obj->GetTransform()->GetScale();

        XMVECTOR bMin = XMVectorSet(pos.x - half * scale.x,
                                    pos.y - half * scale.y,
                                    pos.z - half * scale.z, 0.f);
        XMVECTOR bMax = XMVectorSet(pos.x + half * scale.x,
                                    pos.y + half * scale.y,
                                    pos.z + half * scale.z, 0.f);

        XMVECTOR invD = XMVectorReciprocal(rayDir);
        XMVECTOR t0   = XMVectorMultiply(XMVectorSubtract(bMin, rayOrigin), invD);
        XMVECTOR t1   = XMVectorMultiply(XMVectorSubtract(bMax, rayOrigin), invD);

        XMVECTOR tMin = XMVectorMin(t0, t1);
        XMVECTOR tMax = XMVectorMax(t0, t1);

        float tE = std::max({XMVectorGetX(tMin), XMVectorGetY(tMin), XMVectorGetZ(tMin)});
        float tX = std::min({XMVectorGetX(tMax), XMVectorGetY(tMax), XMVectorGetZ(tMax)});

        if (tE <= tX && tX > 0.f && tE < minT)
        {
            minT   = tE;
            picked = obj;
        }
    }

    m_editorUI->SetSelected(picked); // nullptr이면 선택 해제
}
