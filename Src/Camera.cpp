#include <Windows.h>
#include <DirectXMath.h>
#include <algorithm>

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "InputHandler.h"
#include "GameEngine.h"
#include "RenderManager.h"
#include "DX11Renderer.h"

using namespace DirectX;

Camera::Camera() {}

void Camera::Update(float deltaTime)
{
    InputHandler* input = InputHandler::Instance();
    DX11Renderer* renderer = GameEngine::Instance()->GetRenderManager()->GetRenderer();
    Transform* tr = GetOwner()->GetTransform();

    Vector3 pos = tr->GetPosition();
    Vector3 rot = tr->GetRotation(); // degree

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
        pos += toV3(forward) * wheel * m_zoomSpeed * deltaTime;

    // 가운데 클릭 패닝
    if (input->IsKeyDown(VK_MBUTTON))
    {
        POINT delta = input->GetMouseDelta();
        pos -= toV3(right) * (float)delta.x * m_panSpeed * deltaTime;
        pos += toV3(up) * (float)delta.y * m_panSpeed * deltaTime;

    }

    // 우클릭 회전 + 이동
    if (input->IsKeyDown(VK_RBUTTON))
    {
        POINT delta = input->GetMouseDelta();
        rot.y += (float)delta.x * m_rotateSpeed * deltaTime;
        rot.x += (float)delta.y * m_rotateSpeed * deltaTime;
        rot.x = std::clamp(rot.x, -89.f, 89.f);

        Vector3 move = {};
        if (input->IsKeyDown('W')) move += toV3(forward);
        if (input->IsKeyDown('S')) move -= toV3(forward);
        if (input->IsKeyDown('A')) move -= toV3(right);
        if (input->IsKeyDown('D')) move += toV3(right);
        if (input->IsKeyDown('E')) move += toV3(up);
        if (input->IsKeyDown('Q')) move -= toV3(up);

        pos += move * m_moveSpeed * deltaTime;
    }

    tr->SetPosition(pos);
    tr->SetRotation(rot);

    // 행렬 갱신
    float aspect = (float)renderer->GetWidth() / (float)renderer->GetHeight();
    UpdateMatrix(aspect);
}

void Camera::OnEnable()
{
    GameEngine::Instance()->SetMainCamera(this);
}

void Camera::OnDisable()
{
    if (GameEngine::Instance()->GetMainCamera() == this)
        GameEngine::Instance()->SetMainCamera(nullptr);
}

void Camera::UpdateMatrix(float aspect)
{
    Transform* tr = GetOwner()->GetTransform();
    Vector3 pos = tr->GetPosition();
    Vector3 rot = tr->GetRotation();

    XMMATRIX rotMat = XMMatrixRotationX(XMConvertToRadians(rot.x)) * XMMatrixRotationY(XMConvertToRadians(rot.y));

    XMVECTOR eye = XMVectorSet(pos.x, pos.y, pos.z, 0.f);
    XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotMat);
    XMVECTOR target = XMVectorAdd(eye, forward);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);

    m_view = XMMatrixLookAtLH(eye, target, up);
    m_proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fov), aspect, m_nearZ, m_farZ);

}
