#pragma once
#include "Component.h"
#include "Vector3.h"
#include <DirectXMath.h>

class Transform : public Component
{
public:
	Transform() : m_position(0, 0, 0), m_rotation(0, 0, 0), m_scale(1.f, 1.f, 1.f) {}
	virtual ~Transform() {}

public:
	const Vector3& GetPosition() const { return m_position; }
	const Vector3& GetRotation() const { return m_rotation; }
	const Vector3& GetScale() const { return m_scale; }


	void SetPosition(const Vector3& position) { m_position = position; }
	void SetRotation(const Vector3& rotation) { m_rotation = rotation; }
	void SetScale(const Vector3& scale) { m_scale = scale; }

	// 행렬 변환
	DirectX::XMMATRIX GetWorldMatrix() const
	{
		using namespace DirectX;

		XMMATRIX S = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

		XMMATRIX R = XMMatrixRotationX(XMConvertToRadians(m_rotation.x))
			* XMMatrixRotationY(XMConvertToRadians(m_rotation.y))
			* XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));

		XMMATRIX T = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

		return S * R * T;
	}

	void SetRotationFromQuaternion(DirectX::XMVECTOR quat)
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

private:
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;

};