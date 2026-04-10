#include "Transform.h"

#include "imgui/imgui.h"

#include "ComponentRegistry.h"
#include "GameObject.h"

#include <DirectXMath.h>
using namespace DirectX;

void Transform::AddPosition(Vector3 addPos)
{
	m_position += addPos;
}

// 행렬 변환
DirectX::XMMATRIX Transform::GetWorldMatrix() const
{
	using namespace DirectX;

	XMMATRIX S = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	XMMATRIX R = XMMatrixRotationX(XMConvertToRadians(m_rotation.x))
		* XMMatrixRotationY(XMConvertToRadians(m_rotation.y))
		* XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));

	XMMATRIX T = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	return S * R * T;
}

void Transform::SetRotationFromQuaternion(DirectX::XMVECTOR quat)
{
	using namespace DirectX;
	XMMATRIX rotMat = XMMatrixRotationQuaternion(quat);
	XMFLOAT4X4 m;
	XMStoreFloat4x4(&m, rotMat);

	float pitch = asinf(-m._32);
	float yaw = (cosf(pitch) > 0.0001f) ? atan2f(m._31, m._33) : 0.0f;
	float roll = atan2f(m._12, m._22);

	m_rotation.x = XMConvertToDegrees(pitch);
	m_rotation.y = XMConvertToDegrees(yaw);
	m_rotation.z = XMConvertToDegrees(roll);
}

Vector3 Transform::GetForward() const
{
	// 로컬 전방(0,0,1)을 현재 회전 행렬로 변환
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z));

	XMVECTOR f = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), R);
	return { XMVectorGetX(f), XMVectorGetY(f), XMVectorGetZ(f) };
}

Vector3 Transform::GetRight() const
{
	// 로컬 우측(1,0,0) 변환
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z));

	XMVECTOR r = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), R);
	return { XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) };
}

Vector3 Transform::GetUp() const
{
	// 로컬 상단(0,1,0) 변환
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z));

	XMVECTOR u = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), R);
	return { XMVectorGetX(u), XMVectorGetY(u), XMVectorGetZ(u) };
}

void Transform::LookAt(const Vector3& target)
{
	XMVECTOR eye = XMVectorSet(m_position.x, m_position.y, m_position.z, 1.0f);
	XMVECTOR focus = XMVectorSet(target.x, target.y, target.z, 1.0f);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	// LookAt 행렬 생성 (View 행렬의 역행렬 개념)
	XMMATRIX lookAtMat = XMMatrixLookAtLH(eye, focus, up);
	XMVECTOR det = XMMatrixDeterminant(lookAtMat);
	XMMATRIX worldRotMat = XMMatrixInverse(&det, lookAtMat);

	// 쿼터니언으로 변환 후 오일러 각도로 추출
	XMVECTOR quat = XMQuaternionRotationMatrix(worldRotMat);
	SetRotationFromQuaternion(quat);
}

Vector3 Transform::TransformPoint(const Vector3& localPoint)
{
	XMVECTOR p = XMVectorSet(localPoint.x, localPoint.y, localPoint.z, 1.0f);
	XMVECTOR worldP = XMVector3TransformCoord(p, GetWorldMatrix());
	return { XMVectorGetX(worldP), XMVectorGetY(worldP), XMVectorGetZ(worldP) };
}

Vector3 Transform::InverseTransformPoint(const Vector3& worldPoint)
{
	return Vector3();
}

void Transform::OnEditorGUI()
{
	ImGui::Text("Position"); ImGui::SetNextItemWidth(-1.f);
	ImGui::DragFloat3("##pos", &m_position.x, 0.1f);

	ImGui::Text("Rotation"); ImGui::SetNextItemWidth(-1.f);
	ImGui::DragFloat3("##rot", &m_rotation.x, 1.f);

	ImGui::Text("Scale");    ImGui::SetNextItemWidth(-1.f);
	ImGui::DragFloat3("##scl", &m_scale.x, 0.1f, 0.001f, 1000.f);

}

REGISTER_COMPONENT(Transform, "Core")