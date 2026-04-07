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
    m_obj = GetOwner();
    m_transform = m_obj->GetTransform();
    m_cam = m_obj->GetComponent<Camera>();
}

void EditorCameraScript::Update(float deltaTime)
{
    auto input = InputHandler::Instance();


    //DX11Renderer* renderer = GameEngine::Instance()->GetRenderManager()->GetRenderer();


    Vector3 pos = m_transform->GetPosition();
    Vector3 rot = m_transform->GetRotation(); // degree

    // 방향 벡터 계산
    XMMATRIX rotMat = XMMatrixRotationX(XMConvertToRadians(rot.x)) * XMMatrixRotationY(XMConvertToRadians(rot.y));

    XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotMat);
    XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotMat);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);

    auto toV3 = [](XMVECTOR v) -> Vector3
        {
            return { XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v) };
        };

    // 줌
    float wheel = input->GetMouseWheel();
    if (wheel != 0.f)
        pos += toV3(forward) * wheel * m_cam->m_zoomSpeed * deltaTime;

    // 가운데 클릭 패닝
    if (input->IsKeyDown(VK_MBUTTON))
    {
        POINT delta = input->GetMouseDelta();
        pos -= toV3(right) * (float)delta.x * m_cam->m_panSpeed * deltaTime;
        pos += toV3(up) * (float)delta.y * m_cam->m_panSpeed * deltaTime;

    }

    // 우클릭 회전 + 이동
    if (input->IsKeyDown(VK_RBUTTON))
    {
        POINT delta = input->GetMouseDelta();
        rot.y += (float)delta.x * m_cam->m_rotateSpeed * deltaTime;
        rot.x += (float)delta.y * m_cam->m_rotateSpeed * deltaTime;
        rot.x = std::clamp(rot.x, -89.f, 89.f);

        Vector3 move = {};
        if (input->IsKeyDown('W')) move += toV3(forward);
        if (input->IsKeyDown('S')) move -= toV3(forward);
        if (input->IsKeyDown('A')) move -= toV3(right);
        if (input->IsKeyDown('D')) move += toV3(right);
        if (input->IsKeyDown('E')) move += toV3(up);
        if (input->IsKeyDown('Q')) move -= toV3(up);

        pos += move * m_cam->m_moveSpeed * deltaTime;
    }

    // [수정] renderer->GetWidth() 대신 EditorUI의 SceneViewSize를 사용해야 함
    auto editorUI = GameEngine::Instance()->GetRenderManager()->GetEditorUI();
    ImVec2 sSize = editorUI->GetSceneViewSize();

    // 0으로 나누기 방지
    float aspect = 1.0f;
    if (sSize.y > 0)
        aspect = sSize.x / sSize.y;

    m_cam->UpdateMatrix(aspect);

    m_transform->SetPosition(pos);
    m_transform->SetRotation(rot);


    // 중요: 이동 후 카메라 행렬 갱신
    if (auto cam = GetOwner()->GetComponent<Camera>())
    {
        // SceneView 크기에 맞춘 Aspect Ratio 적용
        auto editorUI = GameEngine::Instance()->GetRenderManager()->GetEditorUI();
        float aspect = editorUI->GetSceneViewSize().x / editorUI->GetSceneViewSize().y;
        cam->UpdateMatrix(aspect);
    }
}