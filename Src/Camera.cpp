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
#include "EditorUI.h"

using namespace DirectX;

Camera::Camera(const std::string& name) : Component(name) {}

void Camera::Update(float deltaTime)
{
}

void Camera::OnEnable()
{
    //GameEngine::Instance()->SetEditorCamera(this);
}

void Camera::OnDisable()
{
    //if (GameEngine::Instance()->GetEditorCamera() == this)
    //    GameEngine::Instance()->SetEditorCamera(nullptr);
}

void Camera::UpdateMatrix(float aspect)
{
    Transform* tr = GetGameObject()->GetTransform();
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

void Camera::BindToGPU()
{
    // 1. 현재 엔진의 렌더러와 컨텍스트를 가져옵니다.
    auto renderer = GameEngine::Instance()->GetRenderManager()->GetRenderer();
    auto context = renderer->GetContext();

    // 2. 쉐이더로 보낼 데이터를 담을 구조체 (보통 프로젝트의 상수 버퍼 구조체와 일치해야 함)
    // 쉐이더(.hlsl)의 cbuffer MatrixBuffer와 순서가 같아야 합니다.
    struct MatrixBuffer
    {
        XMMATRIX view;
        XMMATRIX projection;
    };

    MatrixBuffer cb;
    // DirectX 11 HLSL은 기본적으로 Column-Major이므로 Transpose(전치) 해줍니다.
    cb.view = XMMatrixTranspose(m_view);
    cb.projection = XMMatrixTranspose(m_proj);

    // 3. Renderer가 미리 만들어둔 카메라 전용 상수 버퍼(Constant Buffer)를 업데이트합니다.
    // (만약 Renderer에 UpdateCameraBuffer 같은 함수를 따로 만드셨다면 그걸 호출해도 됩니다.)
    renderer->UpdateCameraConstantBuffer(&cb, sizeof(MatrixBuffer));
}

Ray Camera::ScreenPointToRay(Vector2 screenPos, Vector2 viewSize)
{
    // 1. NDC 변환 (이미 구현하신 로직을 여기로 이동)
    float ndcX = (2.f * screenPos.x / viewSize.x) - 1.f;
    float ndcY = -(2.f * screenPos.y / viewSize.y) + 1.f;

    // 2. 역행렬을 이용한 레이 계산
    XMMATRIX invView = XMMatrixInverse(nullptr, m_view);

    float tanHalfFovX = 1.f / XMVectorGetX(m_proj.r[0]);
    float tanHalfFovY = 1.f / XMVectorGetY(m_proj.r[1]);

    XMVECTOR rayView = XMVectorSet(ndcX * tanHalfFovX, ndcY * tanHalfFovY, 1.f, 0.f);

    Ray ray;
    XMStoreFloat3((XMFLOAT3*)&ray.direction, XMVector3Normalize(XMVector3TransformNormal(rayView, invView)));
    XMStoreFloat3((XMFLOAT3*)&ray.origin, invView.r[3]);

    return ray;
}

void Camera::OnEditorGUI()
{
    ImGui::DragFloat("FOV", &m_fov, 0.5f, 10.f, 170.f);
    ImGui::DragFloat("Near", &m_nearZ, 0.01f, 0.01f, 10.f);
    ImGui::DragFloat("Far", &m_farZ, 1.f, 10.f, 10000.f);
    ImGui::DragFloat("Move Speed", &m_moveSpeed, 0.5f, 0.1f, 100.f);
    ImGui::DragFloat("Rot Speed", &m_rotateSpeed, 0.5f, 1.f, 180.f);
    ImGui::DragFloat("Zoom Speed", &m_zoomSpeed, 0.5f, 1.f, 100.f);
}