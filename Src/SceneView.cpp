#include "SceneView.h"
#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"
#include "RenderPipeline.h"
#include "DX11Renderer.h"
#include "GameEngine.h"
#include "RenderManager.h"
#include "EditorUI.h"
#include "imgui.h"
#include "Gizmo.h"
#include "EditorCameraScript.h"
SceneView::SceneView() : EditorWindow("Scene View") {}
SceneView::~SceneView() {}

void SceneView::Init()
{
    // EditorCam
    m_cameraObject = std::make_unique<GameObject>("EditorCamera");
    m_cameraObject->GetTransform()->SetPosition({ 0.f, 3.f, -8.f });
    m_cameraObject->GetTransform()->SetRotation({ 15.f, 0.f, 0.f });

    m_cameraObject->AddComponent<Camera>();
    m_cameraObject->AddComponent<EditorCameraScript>();

    m_editorCamera = m_cameraObject->GetComponent<Camera>();
    GameEngine::Instance()->SetEditorCamera(m_editorCamera);
}

void SceneView::Update(float deltaTime, RenderPipeline* pipeline, DX11Renderer* renderer)
{
    if (!m_editorCamera || !pipeline || !renderer) return;

    // 에디터 카메라 업데이트 (WASD + 마우스)
    m_cameraObject->Update(deltaTime);

    // Aspect ratio — SceneView 창 크기 기반
    auto* editorUI = GameEngine::Instance()->GetRenderManager()->GetEditorUI();
    ImVec2 sSize = editorUI->GetSceneViewSize();
    float aspect = (sSize.y > 0.f) ? sSize.x / sSize.y : 16.f / 9.f;
    m_editorCamera->UpdateMatrix(aspect);

    // RenderPipeline에 씬 렌더링 예약
    RenderViewRequest req;
    req.targetCamera  = m_editorCamera;
    req.rtv           = renderer->GetSceneRTV();
    req.clearColor[0] = 0.12f;
    req.clearColor[1] = 0.12f;
    req.clearColor[2] = 0.15f;
    req.clearColor[3] = 1.0f;
    req.isEditorMode  = true;

    pipeline->PushRequest(req);
}

void SceneView::OnGUI()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Scene View");
    ImGui::PopStyleVar();

    auto* editorUI = GameEngine::Instance()->GetRenderManager()->GetEditorUI();
    auto* renderer = GameEngine::Instance()->GetRenderManager()->GetRenderer();

    ImVec2 pos  = ImGui::GetCursorScreenPos();
    ImVec2 size = ImGui::GetContentRegionAvail();

    // SceneView 위치/크기를 EditorUI에 저장 (Ray cast, Gizmo 에임 보정용)
    editorUI->SetSceneViewPos(pos);
    editorUI->SetSceneViewSize(size);

    // RTT 결과물 출력
    if (renderer->GetSceneSRV())
        ImGui::Image((void*)renderer->GetSceneSRV(), size);

    // Gizmo — SceneView 이미지 위에 그리기
    Gizmo* gizmo = editorUI->GetGizmo();
    GameObject* selected = editorUI->GetSelected();
    if (gizmo && selected)
    {
        gizmo->SetTarget(selected);
        gizmo->Update();

        ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
        ImGuizmo::SetDrawlist();
        gizmo->Render();
    }

    ImGui::End();
}
