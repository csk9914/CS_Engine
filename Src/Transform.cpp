#include "Transform.h"

#include "imgui/imgui.h"

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

void Transform::OnEditorGUI()
{
	ImGui::Text("Position"); ImGui::SetNextItemWidth(-1.f);
	ImGui::DragFloat3("##pos", &m_position.x, 0.1f);

	ImGui::Text("Rotation"); ImGui::SetNextItemWidth(-1.f);
	ImGui::DragFloat3("##rot", &m_rotation.x, 1.f);

	ImGui::Text("Scale");    ImGui::SetNextItemWidth(-1.f);
	ImGui::DragFloat3("##scl", &m_scale.x, 0.1f, 0.001f, 1000.f);

}
