#include "EditorCameraScript.h"
#include "GameObject.h"
#include "InputHandler.h"
#include "Camera.h"
#include "GameEngine.h"
#include "RenderManager.h"
#include "EditorUI.h"
#include <Windows.h>
#include <DirectXMath.h>
#include <algorithm>

using namespace DirectX;

void EditorCameraScript::Start()
{
    m_obj = GetGameObject();
    m_transform = m_obj->GetTransform();
    m_cam = m_obj->GetComponent<Camera>();
}

void EditorCameraScript::Update(float deltaTime)
{
    if (!m_cam || !m_transform) return;

    auto input = InputHandler::Instance();
    auto editorUI = GameEngine::Instance()->GetRenderManager()->GetEditorUI();

    // 1. 상태 변수들
    bool isHovered = editorUI->IsSceneViewHovered();
    bool isRightDown = input->IsKeyDown(VK_RBUTTON);
    bool isMiddleDown = input->IsKeyDown(VK_MBUTTON);

    // [핵심] 조작 유지 플래그
    static bool s_isStuckInSceneView = false;

    // 2. 조작 시작 판정: 씬 뷰 위에서 버튼을 '막 눌렀을 때'만 점유(Capture) 시작
    if (isHovered && (input->IsKeyOneDown(VK_RBUTTON) || input->IsKeyOneDown(VK_MBUTTON)))
    {
        s_isStuckInSceneView = true;
    }

    // 버튼을 다 떼면 점유 해제
    if (!isRightDown && !isMiddleDown)
    {
        s_isStuckInSceneView = false;
    }

    // --- [수정 포인트] 줌/이동/회전 실행 조건 ---
    // 줌은 마우스가 호버 상태일 때만, 이동/회전은 점유 상태일 때만 동작하도록 가드(Guard) 설정
    bool canZoom = isHovered;
    bool canControl = s_isStuckInSceneView;

    // 아무 조건도 만족하지 않으면 업데이트 중단
    if (!canZoom && !canControl) return;

    // 3. 현재 트랜스폼 및 방향 벡터 계산 (기존과 동일)
    Vector3 pos = m_transform->GetPosition();
    Vector3 rot = m_transform->GetRotation();

    XMMATRIX rotMat = XMMatrixRotationX(XMConvertToRadians(rot.x)) * XMMatrixRotationY(XMConvertToRadians(rot.y));
    XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotMat);
    XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotMat);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);

    auto toV3 = [](XMVECTOR v) -> Vector3 { return { XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v) }; };

    // 4. 입력 처리
    // [줌] - 호버 상태일 때만
    float wheel = input->GetMouseWheel();
    if (canZoom && wheel != 0.f)
    {
        pos += toV3(forward) * wheel * m_cam->m_zoomSpeed * deltaTime;
    }

    // [이동 및 회전] - 점유(canControl) 상태일 때만 실행
    if (canControl)
    {
        // [마우스 휠 클릭 - 패닝]
        if (isMiddleDown)
        {
            POINT delta = input->GetMouseDelta();
            pos -= toV3(right) * (float)delta.x * m_cam->m_panSpeed * deltaTime;
            pos += toV3(up) * (float)delta.y * m_cam->m_panSpeed * deltaTime;
        }

        // [마우스 우클릭 - 회전 및 이동]
        if (isRightDown)
        {
            POINT delta = input->GetMouseDelta();
            rot.y += (float)delta.x * m_cam->m_rotateSpeed * deltaTime;
            rot.x += (float)delta.y * m_cam->m_rotateSpeed * deltaTime;
            rot.x = std::clamp(rot.x, -89.f, 89.f);

            Vector3 move = { 0, 0, 0 };
            if (input->IsKeyDown('W')) move += toV3(forward);
            if (input->IsKeyDown('S')) move -= toV3(forward);
            if (input->IsKeyDown('A')) move -= toV3(right);
            if (input->IsKeyDown('D')) move += toV3(right);
            if (input->IsKeyDown('E')) move += toV3(up);
            if (input->IsKeyDown('Q')) move -= toV3(up);

            pos += move * m_cam->m_moveSpeed * deltaTime;
        }
    }

    // 5. 결과 반영 및 행렬 갱신 (기존과 동일)
    m_transform->SetPosition(pos);
    m_transform->SetRotation(rot);

    ImVec2 sSize = editorUI->GetSceneViewSize();
    float aspect = (sSize.x > 0 && sSize.y > 0) ? (sSize.x / sSize.y) : (16.0f / 9.0f);
    m_cam->UpdateMatrix(aspect);
}